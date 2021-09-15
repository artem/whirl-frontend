#include <whirl/node/rpc/retries.hpp>

#include <whirl/node/rpc/errors.hpp>

#include <timber/log.hpp>

#include <await/futures/util/promise.hpp>

#include <algorithm>
#include <utility>

using wheels::Result;
using namespace await::futures;
using await::executors::IExecutor;
using await::context::StopToken;

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

class Backoff {
 public:
  explicit Backoff(BackoffParams params) : params_(params), next_(params.init) {
  }

  Duration Next() {
    return std::exchange(next_, ComputeNext(next_));
  }

 private:
  Duration ComputeNext(Duration curr) const {
    return std::min<Duration>(curr * params_.factor, params_.max);
  }

 private:
  BackoffParams params_;
  Duration next_;
};

//////////////////////////////////////////////////////////////////////

class Retrier : public std::enable_shared_from_this<Retrier> {
 public:
  Retrier(const IChannelPtr& channel, timber::ILogBackend* log, const Method& method,
          const BytesValue& input, CallOptions options, node::time::ITimeService* time,
          BackoffParams backoff_params)
      : channel_(channel),
        method_(method),
        input_(input),
        options_(std::move(options)),
        time_(std::move(time)),
        backoff_(backoff_params),
        logger_("Retries-Channel", log) {
  }

  Future<BytesValue> Start() {
    auto f_with_retries = promise_.MakeFuture();

    auto self = shared_from_this();

    ++attempt_;
    auto f = channel_->Call(method_, input_, options_);
    auto e = f.GetExecutor();
    SubscribeToResult(std::move(f));

    // Forward executor
    return std::move(f_with_retries).Via(e);
  }

 private:
  void Retry() {
    LOG_INFO("Retry {}.{} request, attempt {}", channel_->Peer(), method_,
                   attempt_);

    ++attempt_;
    auto f = channel_->Call(method_, input_, options_);
    SubscribeToResult(std::move(f));
  }

  void SubscribeToResult(Future<BytesValue> f) {
    auto e = f.GetExecutor();

    auto handler = [self = shared_from_this(),
                    e](Result<BytesValue> result) mutable {
      self->Handle(std::move(result), std::move(e));
    };

    std::move(f).Subscribe(std::move(handler));
  }

  void Handle(Result<BytesValue> result, IExecutor* e) {
    if (result.IsOk() || !IsRetriableError(result.GetError())) {
      std::move(promise_).Set(std::move(result));
    } else {
      ScheduleRetry(std::move(e));
    }
  }

  static bool IsRetriableError(const wheels::Error& error) {
    return error.HasErrorCode() &&
           error.GetErrorCode() == RPCErrorCode::TransportError;
  }

  void ScheduleRetry(IExecutor* e) {
    if (Enough()) {
      Cancel();
      return;
    }

    auto retry = [self = shared_from_this()](Result<void>) {
      self->Retry();
    };

    auto after = time_->After(backoff_.Next());
    std::move(after).Via(e).Subscribe(retry);
  }

  void Cancel() {
    LOG_INFO("Call {}.{} cancelled via stop token, stop retrying",
                   channel_->Peer(), method_);
    std::move(promise_).SetError(wheels::Error(RPCErrorCode::Cancelled));
  }

  bool Enough() const {
    return Cancelled() || AttemptsLimitReached();
  }

  bool Cancelled() const {
    return options_.stop_advice.StopRequested();
  }

  bool AttemptsLimitReached() const {
    return options_.attempts_limit > 0 && attempt_ >= options_.attempts_limit;
  }

 private:
  await::futures::LazyPromise<BytesValue> promise_;

  IChannelPtr channel_;
  Method method_;
  BytesValue input_;

  CallOptions options_;

  node::time::ITimeService* time_;

  size_t attempt_{0};
  Backoff backoff_;

  timber::Logger logger_;
};

//////////////////////////////////////////////////////////////////////

class RetriesChannel : public std::enable_shared_from_this<RetriesChannel>,
                       public IChannel {
 public:
  RetriesChannel(IChannelPtr impl, node::time::ITimeService* time,
                 timber::ILogBackend* log,
                 BackoffParams backoff_params)
      : impl_(std::move(impl)), time_(time), log_(log), backoff_params_(backoff_params) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallOptions options) override {
    auto retrier = std::make_shared<Retrier>(
        impl_, log_, method, input, std::move(options), time_, backoff_params_);
    return retrier->Start();
  }

 private:
  IChannelPtr impl_;
  node::time::ITimeService* time_;
  timber::ILogBackend* log_;
  BackoffParams backoff_params_;
};

//////////////////////////////////////////////////////////////////////

IChannelPtr WithRetries(IChannelPtr channel, node::time::ITimeService* time,
                        timber::ILogBackend* log,
                        BackoffParams backoff_params) {
  return std::make_shared<RetriesChannel>(std::move(channel), time, log,
                                          backoff_params);
}

}  // namespace whirl::rpc
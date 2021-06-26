#include <whirl/rpc/retries.hpp>

#include <whirl/rpc/request_context.hpp>
#include <whirl/rpc/errors.hpp>

#include <whirl/logger/log.hpp>

#include <await/futures/util/promise.hpp>

#include <algorithm>
#include <utility>

using wheels::Result;
using namespace await::futures;
using await::util::StopToken;

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

class Backoff {
 public:
  Backoff(BackoffParams params) : params_(params), next_(params.init) {
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

using Scope = std::weak_ptr<RequestContext>;

//////////////////////////////////////////////////////////////////////

class Retrier : public std::enable_shared_from_this<Retrier> {
 public:
  Retrier(const IChannelPtr& channel, const Method& method,
          const BytesValue& input, CallContext ctx, ITimeServicePtr time,
          BackoffParams backoff_params)
      : channel_(channel),
        method_(method),
        input_(input),
        ctx_(std::move(ctx)),
        time_(std::move(time)),
        backoff_(backoff_params) {
  }

  Future<BytesValue> Start() {
    auto f_with_retries = promise_.MakeFuture();

    auto self = shared_from_this();

    ++attempt_;
    auto f = channel_->Call(method_, input_, ctx_);
    auto e = f.GetExecutor();
    SubscribeToResult(std::move(f));

    // Forward executor
    return std::move(f_with_retries).Via(e);
  }

 private:
  void Retry() {
    WHIRL_LOG_INFO("Retry {}.{} request, attempt {}", channel_->Peer(), method_,
                   attempt_);

    ++attempt_;
    auto f = channel_->Call(method_, input_, ctx_);
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

  void Handle(Result<BytesValue> result, IExecutorPtr e) {
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

  void ScheduleRetry(IExecutorPtr e) {
    if (ctx_.stop_token.StopRequested()) {
      WHIRL_LOG_INFO("Call {}.{} cancelled via stop token, stop retrying",
                     channel_->Peer(), method_);
      std::move(promise_).SetError(wheels::Error(RPCErrorCode::Cancelled));
      return;
    }

    auto retry = [self = shared_from_this()](Result<void>) {
      self->Retry();
    };

    auto after = time_->After(backoff_.Next());
    std::move(after).Via(e).Subscribe(retry);
  }

 private:
  await::futures::LazyPromise<BytesValue> promise_;

  IChannelPtr channel_;
  Method method_;
  BytesValue input_;

  CallContext ctx_;

  ITimeServicePtr time_;

  size_t attempt_{0};
  Backoff backoff_;

  Logger logger_{"Retries-Channel"};
};

//////////////////////////////////////////////////////////////////////

class RetriesChannel : public std::enable_shared_from_this<RetriesChannel>,
                       public IChannel {
 public:
  RetriesChannel(IChannelPtr impl, ITimeServicePtr time,
                 BackoffParams backoff_params)
      : impl_(std::move(impl)),
        time_(std::move(time)),
        backoff_params_(backoff_params) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallContext ctx) override {
    auto retrier = std::make_shared<Retrier>(
        impl_, method, input, std::move(ctx), time_, backoff_params_);
    return retrier->Start();
  }

 private:
  IChannelPtr impl_;
  ITimeServicePtr time_;
  BackoffParams backoff_params_;
};

IChannelPtr WithRetries(IChannelPtr channel, ITimeServicePtr time,
                        BackoffParams backoff_params) {
  return std::make_shared<RetriesChannel>(std::move(channel), std::move(time),
                                          backoff_params);
}

}  // namespace whirl::rpc

#include <whirl/engines/matrix/channels/retries.hpp>

#include <whirl/rpc/request_context.hpp>
#include <whirl/rpc/errors.hpp>

#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/log/logger.hpp>

#include <whirl/helpers/weak_ptr.hpp>

#include <await/futures/util/promise.hpp>

#include <algorithm>

namespace whirl {

static Logger logger_{"Retries-Channel"};

using namespace rpc;
using wheels::Result;
using namespace await::futures;

//////////////////////////////////////////////////////////////////////

class Backoff {
 public:
  Backoff() {
  }

  Duration Next() {
    auto d = next_;
    next_ = std::min<Duration>(next_ * 2, 1000);
    return d;
  }

 private:
  Duration next_{50};
};

//////////////////////////////////////////////////////////////////////

using Scope = std::weak_ptr<RequestContext>;

//////////////////////////////////////////////////////////////////////

class Retrier : public std::enable_shared_from_this<Retrier> {
 public:
  Retrier(const IChannelPtr& channel, const Method& method,
          const BytesValue& input, Scope scope, ITimeServicePtr time)
      : channel_(channel),
        method_(method),
        input_(input),
        scope_(std::move(scope)),
        time_(time) {
  }

  Future<BytesValue> Start() {
    auto f_with_retries = promise_.MakeFuture();

    auto self = shared_from_this();

    ++attempt_;
    auto f = channel_->Call(method_, input_);
    auto e = f.GetExecutor();
    SubscribeToResult(std::move(f));

    // Forward executor
    return std::move(f_with_retries).Via(e);
  }

 private:
  void Retry() {
    WHIRL_SIM_LOG("Retry {}.{} request, attempt {}", channel_->Peer(), method_,
                  attempt_);

    ++attempt_;
    auto f = channel_->Call(method_, input_);
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
    if (IsExpired(scope_)) {
      WHIRL_SIM_LOG("Context for {}.{} expired, stop retrying",
                    channel_->Peer(), method_);
      std::move(promise_).SetError(wheels::Error(RPCErrorCode::TransportError));
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

  Scope scope_;

  ITimeServicePtr time_;

  size_t attempt_{0};
  Backoff backoff_;
};

//////////////////////////////////////////////////////////////////////

class RetriesChannel : public std::enable_shared_from_this<RetriesChannel>,
                       public IChannel {
 public:
  RetriesChannel(IChannelPtr impl, ITimeServicePtr time)
      : impl_(std::move(impl)), time_(std::move(time)) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const Method& method,
                          const BytesValue& input) override {
    Scope scope = GetRequestContext();
    auto retrier = std::make_shared<Retrier>(impl_, method, input,
                                             std::move(scope), time_);
    return retrier->Start();
  }

 private:
  IChannelPtr impl_;
  ITimeServicePtr time_;
};

IChannelPtr WithRetries(IChannelPtr channel, ITimeServicePtr time) {
  return std::make_shared<RetriesChannel>(std::move(channel), std::move(time));
}

}  // namespace whirl

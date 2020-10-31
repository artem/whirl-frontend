#include <whirl/matrix/channels/retries.hpp>

#include <whirl/rpc/impl/request_context.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/services/time.hpp>

#include <whirl/matrix/log/logger.hpp>

#include <whirl/helpers/weak_ptr.hpp>

#include <await/futures/promise.hpp>

#include <algorithm>

namespace whirl {

static Logger logger_{"Retries channel"};

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

using Scope = std::weak_ptr<rpc::RequestContext>;

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
    auto with_retries = std::move(promise_).MakeFuture();

    auto self = shared_from_this();

    ++attempt_;
    auto f = channel_->Call(method_, input_);
    auto e = f.GetExecutor();
    SubscribeToResult(std::move(f));

    // Forward executor
    return std::move(with_retries).Via(e);
  }

 private:
  void Call() {
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
    if (result.IsOk()) {
      std::move(promise_).Set(std::move(result));
    } else {
      Retry(std::move(e));
    }
  }

  void Retry(IExecutorPtr e) {
    if (IsExpired(scope_)) {
      WHIRL_FMT_LOG("Context for {}.{} expired, stop retrying",
                    channel_->Peer(), method_);
      std::move(promise_).SetError(Error(RPCErrorCode::TransportError));
      return;
    }

    auto retry = [this, self = shared_from_this()](Result<void>) {
      WHIRL_FMT_LOG("Retry {}.{} request, attempt {}", channel_->Peer(),
                    method_, attempt_);
      self->Call();
    };

    auto after = time_->After(backoff_.Next());
    std::move(after).Via(e).Subscribe(retry);
  }

 private:
  Promise<BytesValue> promise_;

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
                       public rpc::IChannel {
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
    Scope scope = rpc::GetRequestContext();
    auto retrier = std::make_shared<Retrier>(impl_, method, input,
                                             std::move(scope), time_);
    return retrier->Start();
  }

 private:
  rpc::IChannelPtr impl_;
  ITimeServicePtr time_;
};

rpc::IChannelPtr WithRetries(rpc::IChannelPtr channel, ITimeServicePtr time) {
  return std::make_shared<RetriesChannel>(std::move(channel), std::move(time));
}

}  // namespace whirl

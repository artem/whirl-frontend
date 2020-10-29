#include <whirl/matrix/channels/logging.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/matrix/log/logger.hpp>
#include <whirl/matrix/world/global.hpp>

#include <whirl/services/time.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>
#include <whirl/rpc/impl/context.hpp>

#include <whirl/helpers/weak_ptr.hpp>

namespace whirl {

static Logger logger_{"Retries channel"};

using namespace rpc;
using wheels::Result;
using namespace await::futures;

struct WeakContext {
  static WeakContext Get() {
    if (auto ctx = rpc::GetContext()) {
      return WeakContext(ctx);
    } else {
      return WeakContext();
    }
  }

  bool NotEmpty() const {
    return !empty_;
  }

  bool Expired() const {
    return ctx_.expired();
  }

 private:
  WeakContext(ContextPtr ctx)
    : empty_{false}, ctx_(ctx) {
  }

  WeakContext()
    : empty_{true} {
  }

 private:
  bool empty_;
  std::weak_ptr<rpc::Context> ctx_;
};

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

    auto ctx = WeakContext::Get();
    return CallImpl(method, input, InitDelay(), ctx);
  }

 private:
  Future<BytesValue> CallImpl(const Method& method,
                          const BytesValue& input, Duration delay, WeakContext ctx) {

    auto f = impl_->Call(method, input);

    auto self = this->shared_from_this();

    auto retry = [this, self, method, input, delay, ctx](Result<void>) -> Future<BytesValue> {
      if (ctx.NotEmpty() && ctx.Expired()) {
        //WHIRL_FMT_LOG("Request context expired, stop retrying");
        return MakeError(Error(RPCErrorCode::TransportError));
      }

      WHIRL_FMT_LOG("Retry {}.{} request", self->Peer(), method);
      return self->CallImpl(method, input, NextDelay(delay), ctx);
    };

    auto ex = f.GetExecutor();

    auto fallback = [time = time_, delay, ex,
                     retry = std::move(retry)](const Error& e) mutable {
      if (e.GetErrorCode() == RPCErrorCode::TransportError) {
        return time->After(delay).Via(ex).Then(retry);
      } else {
        return await::futures::MakeError(Error(e)).As<BytesValue>();
      }
    };

    return await::futures::RecoverWith(std::move(f), fallback);
  }

 private:
  Duration NextDelay(Duration delay) const{
    return delay * 2;
  }

  Duration InitDelay() const {
    return GlobalRandomNumber(90, 110);
  }

 private:
  rpc::IChannelPtr impl_;
  ITimeServicePtr time_;
};

rpc::IChannelPtr WithRetries(rpc::IChannelPtr channel, ITimeServicePtr time) {
  return std::make_shared<RetriesChannel>(std::move(channel), std::move(time));
}

}  // namespace whirl

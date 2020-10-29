#include <whirl/matrix/channels/logging.hpp>

#include <whirl/rpc/impl/request_context.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/services/time.hpp>

#include <whirl/matrix/log/logger.hpp>
#include <whirl/matrix/world/global.hpp>

#include <whirl/helpers/weak_ptr.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

namespace whirl {

static Logger logger_{"Retries channel"};

using namespace rpc;
using wheels::Result;
using namespace await::futures;

using WeakRequestContextRef = std::weak_ptr<rpc::RequestContext>;


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

    WeakRequestContextRef context = rpc::GetRequestContext();
    return CallImpl(method, input, InitDelay(), context);
  }

 private:
  Future<BytesValue> CallImpl(const Method& method,
                          const BytesValue& input, Duration delay, WeakRequestContextRef context) {

    auto f = impl_->Call(method, input);

    auto self = this->shared_from_this();

    auto retry = [this, self, method, input, delay, context](Result<void>) -> Future<BytesValue> {
      if (IsExpired(context)) {
        WHIRL_FMT_LOG("Request context for {} expired, stop retrying", method);
        return MakeError(Error(RPCErrorCode::TransportError));
      }

      WHIRL_FMT_LOG("Retry {}.{} request", self->Peer(), method);
      return self->CallImpl(method, input, NextDelay(delay), context);
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

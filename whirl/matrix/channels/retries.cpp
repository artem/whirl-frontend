#include <whirl/matrix/channels/logging.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/matrix/log/logger.hpp>

#include <whirl/services/time.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

namespace whirl {

static Logger logger_{"Logging channel"};

using namespace rpc;
using wheels::Result;
using namespace await::futures;

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
    auto f = impl_->Call(method, input);

    auto self = this->shared_from_this();

    auto retry = [self, method, input]() { return self->Call(method, input); };

    auto fallback = [time = time_,
                     retry = std::move(retry)](const Error& e) mutable {
      if (e.GetErrorCode() == RPCErrorCode::TransportError) {
        // auto after = time_service->After(10);
        // std::move(after).Then(retry);
        return retry();
      } else {
        return await::futures::MakeError(Error(e)).As<BytesValue>();
      }
    };

    return await::futures::RecoverWith(std::move(f), fallback);
  }

 private:
  rpc::IChannelPtr impl_;
  ITimeServicePtr time_;
};

rpc::IChannelPtr WithRetries(rpc::IChannelPtr channel,
                             ITimeServicePtr time) {
  return std::make_shared<RetriesChannel>(std::move(channel), std::move(time));
}

}  // namespace whirl

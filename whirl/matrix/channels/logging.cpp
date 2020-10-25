#include <whirl/matrix/channels/logging.hpp>

#include <whirl/rpc/impl/channel.hpp>

#include <whirl/matrix/log/logger.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

#include <vector>

namespace whirl {

static Logger logger_{"Logging channel"};

using namespace rpc;
using wheels::Result;

class LoggingChannel : public rpc::IRPCChannel {
 public:
  LoggingChannel(IRPCChannelPtr impl) : impl_(std::move(impl)) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const Callee& callee,
                          const BytesValue& input) override {
    auto f = impl_->Call(callee, input);

    auto log = [callee,
                peer = Peer()](const Result<BytesValue>& result) mutable {
      if (result.IsOk()) {
        WHIRL_FMT_LOG("Method {}.{} completed: Ok", peer, callee.ToString());
      } else {
        WHIRL_FMT_LOG("Method {}.{} failed: {}", peer, callee.ToString(),
                      result.GetErrorCode().message());
      }
    };

    return await::futures::SubscribeConst(std::move(f), std::move(log));
  }

 private:
  rpc::IRPCChannelPtr impl_;
};

rpc::IRPCChannelPtr MakeLoggingChannel(rpc::IRPCChannelPtr channel) {
  return std::make_shared<LoggingChannel>(std::move(channel));
}

}  // namespace whirl

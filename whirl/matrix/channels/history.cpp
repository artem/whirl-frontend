#include <whirl/matrix/channels/history.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

namespace whirl {

using namespace rpc;
using wheels::Result;

class HistoryChannel : public rpc::IRPCChannel {
 public:
  HistoryChannel(IRPCChannelPtr impl) : impl_(std::move(impl)) {
  }

  void Start() override {
    // Nop
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const std::string& method,
                          const BytesValue& input) override {
    size_t call_id = GetHistoryRecorder().CallStarted(method, input);

    auto f = impl_->Call(method, input);

    auto record = [call_id](const Result<BytesValue>& result) mutable {
      HandleCallResult(call_id, result);
    };

    return await::futures::SubscribeConst(std::move(f), std::move(record));
  }

 private:
  static void HandleCallResult(size_t call_id,
                               const Result<BytesValue>& result) {
    auto& recorder = GetHistoryRecorder();

    if (result.IsOk()) {
      recorder.CallCompleted(call_id, result.ValueUnsafe());
    } else {
      if (MaybeCompleted(result.GetErrorCode())) {
        recorder.CallMaybeCompleted(call_id);
      } else {
        recorder.Remove(call_id);
      }
    }
  }

  static bool MaybeCompleted(const std::error_code e) {
    return e == RPCErrorCode::TransportError ||
           e == RPCErrorCode::ExecutionError;
  }

 private:
  rpc::IRPCChannelPtr impl_;
};

rpc::IRPCChannelPtr MakeHistoryChannel(rpc::IRPCChannelPtr channel) {
  return std::make_shared<HistoryChannel>(std::move(channel));
}

}  // namespace whirl

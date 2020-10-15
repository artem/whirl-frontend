#include <whirl/matrix/channels/history.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/history/recorder.hpp>

#include <await/futures/promise.hpp>

namespace whirl {

using namespace rpc;
using wheels::Result;

static histories::Recorder& AccessHistoryRecorder() {
  return World::Access()->HistoryRecorder();
}

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
    size_t call_id = AccessHistoryRecorder().CallStarted(method, input);

    auto f = impl_->Call(method, input);

    auto e = f.GetExecutor();

    auto [_f, _p] = await::futures::MakeContract<BytesValue>();

    auto record = [_p = std::move(_p),
                   call_id](Result<BytesValue> result) mutable {
      HandleCallResult(call_id, result);
      std::move(_p).Set(std::move(result));
    };

    std::move(f).Subscribe(std::move(record));
    return std::move(_f).Via(e);
  }

 private:
  static void HandleCallResult(size_t call_id,
                               const Result<BytesValue>& result) {
    auto& recorder = AccessHistoryRecorder();

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

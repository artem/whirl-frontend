#include <whirl/matrix/channels/history.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

namespace whirl {

using namespace rpc;
using wheels::Result;

using Cookie = histories::Recorder::Cookie;

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
    auto cookie = GetHistoryRecorder().CallStarted(method, input);

    auto f = impl_->Call(method, input);

    auto record = [cookie](const Result<BytesValue>& result) mutable {
      RecordCallResult(cookie, result);
    };

    return await::futures::SubscribeConst(std::move(f), std::move(record));
  }

 private:
  static void RecordCallResult(Cookie cookie,
                               const Result<BytesValue>& result) {
    auto& recorder = GetHistoryRecorder();

    auto trace_id = TryGetCurrentTraceId();
    if (trace_id.has_value()) {
      recorder.AddLabel(cookie, *trace_id);
    }

    if (result.IsOk()) {
      recorder.CallCompleted(cookie, result.ValueUnsafe());
    } else {
      if (MaybeCompleted(result.GetErrorCode())) {
        recorder.CallMaybeCompleted(cookie);
      } else {
        recorder.RemoveCall(cookie);
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

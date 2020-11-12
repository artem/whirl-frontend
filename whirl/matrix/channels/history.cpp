#include <whirl/matrix/channels/history.hpp>

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/errors.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <whirl/matrix/world/global/global.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

namespace whirl {

using namespace rpc;
using wheels::Result;

using Cookie = histories::Recorder::Cookie;

class HistoryChannel : public IChannel {
 public:
  HistoryChannel(IChannelPtr impl) : impl_(std::move(impl)) {
  }

  void Close() override {
    impl_->Close();
  }

  const std::string& Peer() const override {
    return impl_->Peer();
  }

  Future<BytesValue> Call(const Method& method,
                          const BytesValue& input) override {
    auto cookie = GetHistoryRecorder().CallStarted(method.name, input);

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

    if (auto trace_id = TryGetCurrentTraceId()) {
      recorder.AddLabel(cookie, *trace_id);
    }

    if (result.IsOk()) {
      recorder.CallCompleted(cookie, result.ValueUnsafe());
    } else {
      if (MaybeCompleted(result.GetErrorCode())) {
        recorder.CallLost(cookie);
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
  IChannelPtr impl_;
};

IChannelPtr MakeHistoryChannel(IChannelPtr channel) {
  return std::make_shared<HistoryChannel>(std::move(channel));
}

}  // namespace whirl

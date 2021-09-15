#include <whirl/engines/matrix/history/channel.hpp>

#include <whirl/node/rpc/channel.hpp>
#include <whirl/node/rpc/errors.hpp>
#include <whirl/node/rpc/trace.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

#include <await/futures/core/future.hpp>
#include <await/futures/helpers.hpp>

using namespace whirl::rpc;
using await::futures::Future;
using wheels::Result;

namespace whirl::matrix {

using Cookie = HistoryRecorder::Cookie;

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

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallOptions options) override {
    auto cookie = GetHistoryRecorder().CallStarted(method.name, input);

    auto f = impl_->Call(method, input, std::move(options));

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

}  // namespace whirl::matrix

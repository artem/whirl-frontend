#include <whirl/matrix/channels/history.hpp>

#include <whirl/rpc/impl/channel.hpp>

#include <whirl/matrix/world/world.hpp>
#include <whirl/matrix/history/recorder.hpp>

#include <await/futures/promise.hpp>

namespace whirl {

static history::Recorder& AccessHistoryRecorder() {
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
    size_t call_id = AccessHistoryRecorder().StartCall(method, input);

    auto f = impl_->Call(method, input);

    auto e = f.GetExecutor();

    auto [_f, _p] = await::futures::MakeContract<BytesValue>();

    auto record = [_p = std::move(_p),
                   call_id](Result<BytesValue> result) mutable {
      AccessHistoryRecorder().CompleteCall(call_id, result.Value());
      std::move(_p).Set(std::move(result));
    };

    std::move(f).Subscribe(std::move(record));
    return std::move(_f).Via(e);
  }

 private:
  rpc::IRPCChannelPtr impl_;
};

rpc::IRPCChannelPtr MakeHistoryChannel(rpc::IRPCChannelPtr channel) {
  return std::make_shared<HistoryChannel>(std::move(channel));
}

}  // namespace whirl

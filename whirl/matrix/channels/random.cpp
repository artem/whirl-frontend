#include <whirl/matrix/channels/random.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl {

using namespace rpc;

using ChannelVector = std::vector<IRPCChannelPtr>;

static const std::string kRandomPeer = "random";

class RandomChannel : public rpc::IRPCChannel {
 public:
  RandomChannel(ChannelVector channels) : channels_(std::move(channels)) {
  }

  ~RandomChannel() {
    Close();
  }

  void Start() override {
    // Nop
  }

  Future<BytesValue> Call(const std::string& method,
                          const BytesValue& input) override {
    size_t index = GlobalRandomNumber(channels_.size());
    return channels_[index]->Call(method, input);
  }

  const std::string& Peer() const override {
    return kRandomPeer;
  }

  void Close() override {
    for (auto& channel : channels_) {
      channel->Close();
    }
    channels_.clear();
  }

 private:
  ChannelVector channels_;
};

rpc::IRPCChannelPtr MakeRandomChannel(ChannelVector&& channels) {
  return std::make_shared<RandomChannel>(std::move(channels));
}

}  // namespace whirl
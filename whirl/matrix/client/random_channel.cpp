#include <whirl/matrix/client/random_channel.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl {

using namespace rpc;

using ChannelVector = std::vector<IRPCChannelPtr>;

class RandomChannel : public rpc::IRPCChannel {
 public:
  RandomChannel(ChannelVector channels)
    : channels_(std::move(channels)) {
  }

  ~RandomChannel() {
    Close();
  }

  void Start() override {
    // Nop
  }

  Future<RPCBytes> Call(const std::string& method, const RPCBytes& input) override {
    size_t index = GlobalRandomNumber(channels_.size());
    return channels_[index]->Call(method, input);
  }

  const std::string& Peer() const override {
    static const std::string kRandomPeer = "random";
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
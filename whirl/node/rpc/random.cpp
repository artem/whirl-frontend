#include <whirl/node/rpc/random.hpp>

using await::futures::Future;

using commute::rpc::IChannel;
using commute::rpc::IChannelPtr;
using commute::rpc::Method;
using commute::rpc::BytesValue;
using commute::rpc::CallOptions;

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

static const std::string kRandomPeer = "Random";

class RandomChannel : public IChannel {
 public:
  RandomChannel(std::vector<IChannelPtr> channels, node::IRandomService* random)
      : channels_(std::move(channels)), random_(std::move(random)) {
  }

  ~RandomChannel() {
    Close();
  }

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallOptions options) override {
    size_t index = SelectIndex();
    return channels_[index]->Call(method, input, std::move(options));
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
  size_t SelectIndex() const {
    return random_->GenerateNumber(channels_.size());
  }

 private:
  std::vector<IChannelPtr> channels_;
  node::IRandomService* random_;
};

//////////////////////////////////////////////////////////////////////

IChannelPtr MakeRandomChannel(std::vector<IChannelPtr>&& channels,
                              node::IRandomService* random) {
  return std::make_shared<RandomChannel>(std::move(channels),
                                         std::move(random));
}

}  // namespace whirl::rpc

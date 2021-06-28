#include <whirl/rpc/random.hpp>

using namespace whirl::rpc;
using await::futures::Future;
using await::util::StopToken;

namespace whirl::matrix {

using ChannelVector = std::vector<IChannelPtr>;

static const std::string kRandomPeer = "Random";

class RandomChannel : public IChannel {
 public:
  RandomChannel(ChannelVector channels, IRandomServicePtr random) : channels_(std::move(channels)), random_(std::move(random)) {
  }

  ~RandomChannel() {
    Close();
  }

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallContext ctx) override {
    size_t index = SelectIndex();
    return channels_[index]->Call(method, input, std::move(ctx));
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
    return random_->RandomNumber() % channels_.size();
  }

 private:
  ChannelVector channels_;
  IRandomServicePtr random_;
};

IChannelPtr MakeRandomChannel(ChannelVector&& channels, IRandomServicePtr random) {
  return std::make_shared<RandomChannel>(std::move(channels), std::move(random));
}

}  // namespace whirl::matrix
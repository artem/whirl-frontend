#include <whirl/rpc/round_robin.hpp>

using await::futures::Future;

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

static const std::string kPeer = "RR";

class RRChannel : public IChannel {
 public:
  RRChannel(std::vector<IChannelPtr> channels)
      : channels_(std::move(channels)) {
  }

  ~RRChannel() {
    Close();
  }

  Future<BytesValue> Call(const Method& method, const BytesValue& input,
                          CallOptions options) override {
    auto f = channels_[next_index_]->Call(method, input, std::move(options));
    next_index_ = (next_index_ + 1) % channels_.size();
    return f;
  }

  const std::string& Peer() const override {
    return kPeer;
  }

  void Close() override {
    for (auto& channel : channels_) {
      channel->Close();
    }
    channels_.clear();
  }

 private:
  std::vector<IChannelPtr> channels_;
  size_t next_index_{0};
};

//////////////////////////////////////////////////////////////////////

IChannelPtr MakeRoundRobinChannel(std::vector<IChannelPtr> channels) {
  return std::make_shared<RRChannel>(std::move(channels));
}

}  // namespace whirl::rpc

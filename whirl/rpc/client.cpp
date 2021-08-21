#include <whirl/rpc/client.hpp>

#include <whirl/rpc/transport_channel.hpp>

using await::executors::IExecutorPtr;

namespace whirl::rpc {

class Client : public IClient {
 public:
  Client(ITransport* t, IExecutorPtr e) : t_(t), e_(std::move(e)) {
  }

  IChannelPtr Dial(const std::string& peer) override {
    return std::make_shared<TransportChannel>(t_, e_, peer);
  }

 private:
  ITransport* t_;
  IExecutorPtr e_;
};

IClientPtr MakeClient(ITransport* t, IExecutorPtr e) {
  return std::make_shared<Client>(t, e);
}

}  // namespace whirl::rpc

#include <whirl/rpc/client.hpp>

#include <whirl/rpc/transport_channel.hpp>

using await::executors::IExecutor;

namespace whirl::rpc {

class Client : public IClient {
 public:
  Client(ITransport* t, IExecutor* e) : t_(t), e_(e) {
  }

  IChannelPtr Dial(const std::string& peer) override {
    return std::make_shared<TransportChannel>(t_, e_, peer);
  }

 private:
  ITransport* t_;
  IExecutor* e_;
};

IClientPtr MakeClient(ITransport* t, IExecutor* e) {
  return std::make_shared<Client>(t, e);
}

}  // namespace whirl::rpc

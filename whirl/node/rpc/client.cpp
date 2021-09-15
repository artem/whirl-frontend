#include <whirl/node/rpc/client.hpp>

#include <whirl/node/rpc/transport_channel.hpp>

using await::executors::IExecutor;
using timber::ILogBackend;
using whirl::node::net::ITransport;

namespace whirl::rpc {

class Client : public IClient {
 public:
  Client(ITransport* t, IExecutor* e, ILogBackend* log)
      : t_(t), e_(e), log_(log) {
  }

  IChannelPtr Dial(const std::string& peer) override {
    return std::make_shared<TransportChannel>(t_, e_, log_, peer);
  }

 private:
  ITransport* t_;
  IExecutor* e_;
  ILogBackend* log_;
};

IClientPtr MakeClient(ITransport* t, IExecutor* e, ILogBackend* log) {
  return std::make_shared<Client>(t, e, log);
}

}  // namespace whirl::rpc

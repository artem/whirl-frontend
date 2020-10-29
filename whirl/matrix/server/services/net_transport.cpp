#include <whirl/matrix/server/services/net_transport.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LightTransportSocket : public ITransportSocket {
 public:
  LightTransportSocket(LightNetSocket socket)
      : socket_(std::move(socket)) {
  }

  void Send(const std::string& message) override {
    socket_.Send(message);
  }

  const std::string& Peer() const override {
    return socket_.Peer();
  }

  bool IsConnected() const override {
    return true;
  }

  void Close() override {
    // Nop
  }

 private:
  LightNetSocket socket_;
};

//////////////////////////////////////////////////////////////////////

void NetTransportServer::HandleMessage(const Message& message,
                                       LightNetSocket back) {
  auto g = heap_.Use();

  handler_->HandleMessage(message,
                          std::make_shared<LightTransportSocket>(back));
}

}  // namespace whirl

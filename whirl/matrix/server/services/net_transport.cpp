#include <whirl/matrix/server/services/net_transport.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LightTransportSocket : public ITransportSocket {
 public:
  LightTransportSocket(LightNetSocket socket, const std::string& peer)
      : socket_(std::move(socket)), peer_(peer) {
  }

  void Send(const std::string& message) override {
    socket_.Send(message);
  }

  const std::string& Peer() const override {
    return peer_;
  }

  bool IsConnected() const override {
    return true;
  }

  void Close() override {
    // Nop
  }

 private:
  LightNetSocket socket_;
  std::string peer_;
};

//////////////////////////////////////////////////////////////////////

void NetTransportServer::HandleMessage(const Message& message, LightNetSocket back) {
  auto g = heap_.Use();

  handler_->HandleMessage(message,
      std::make_shared<LightTransportSocket>(back, "?"));
}

}  // namespace whirl

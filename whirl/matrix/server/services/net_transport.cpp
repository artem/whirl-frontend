#include <whirl/matrix/server/services/net_transport.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class ReplyTransportSocket : public ITransportSocket {
 public:
  ReplyTransportSocket(net::ReplySocket socket) : socket_(socket) {
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
  net::ReplySocket socket_;
};

//////////////////////////////////////////////////////////////////////

void NetTransportServer::HandleMessage(const net::Message& message,
                                       net::ReplySocket back) {
  if (auto handler = handler_.lock()) {
    handler->HandleMessage(message,
                           std::make_shared<ReplyTransportSocket>(back));
  }
}

}  // namespace whirl

#pragma once

#include <whirl/rpc/net_transport.hpp>

#include <whirl/matrix/network/transport.hpp>

namespace whirl {

using namespace rpc;

//////////////////////////////////////////////////////////////////////

class NetTransportSocket : public ITransportSocket, public net::ISocketHandler {
 public:
  NetTransportSocket(net::Transport& transport, std::string peer, net::Port port,
                     ITransportHandlerPtr handler)
      : socket_(transport.ConnectTo({peer, port}, this)),
        handler_(handler) {
  }

  ~NetTransportSocket() {
    if (socket_.IsValid()) {
      socket_.Close();
    }
  }

  // ITransportSocket

  void Send(const TransportMessage& message) override {
    socket_.Send(message);
  }

  const std::string& Peer() const override {
    return socket_.Peer();
  }

  bool IsConnected() const override {
    return socket_.IsValid();
  }

  void Close() override {
    socket_.Close();
  }

  // INetSocketHandler

  void HandleMessage(const std::string& message,
                     net::ReplySocket /*back*/) override {
    if (auto handler = handler_.lock()) {
      handler->HandleMessage(message, nullptr);
    }
  }

  void HandleDisconnect(const std::string& peer) override {
    socket_.Close();
    if (auto handler = handler_.lock()) {
      handler->HandleDisconnect(peer);
    }
  }

 private:
  net::ClientSocket socket_;
  ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportServer : public ITransportServer, public net::ISocketHandler {
 public:
  NetTransportServer(net::Transport& transport, net::Port port, ITransportHandlerPtr handler)
      : server_socket_(transport.Serve(port, this)),
        handler_(handler) {
  }

  // ITransportServer

  void Shutdown() override {
    server_socket_.Close();
  }

  // net::ISocketHandler

  void HandleMessage(const net::Message& message,
                     net::ReplySocket back) override;

  void HandleDisconnect(const std::string& client) override {
    if (auto handler = handler_.lock()) {
      handler->HandleDisconnect(client);
    }
  }

 private:
  net::ServerSocket server_socket_;
  ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

struct NetTransport : public ITransport {
 public:
  NetTransport(net::Transport& impl, net::Port port) : impl_(impl), port_(port) {
  }

  ITransportServerPtr Serve(ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportServer>(impl_, port_, handler);
  }

  ITransportSocketPtr ConnectTo(const std::string& peer,
                                ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportSocket>(impl_, peer, port_, handler);
  }

 private:
  net::Transport& impl_;
  net::Port port_;
};

}  // namespace whirl

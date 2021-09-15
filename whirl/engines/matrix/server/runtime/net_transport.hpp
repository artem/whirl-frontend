#pragma once

#include <whirl/net/transport.hpp>

#include <whirl/engines/matrix/network/transport.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class NetTransportSocket : public node::net::ITransportSocket, public net::ISocketHandler {
 public:
  NetTransportSocket(net::Transport& transport, std::string host,
                     net::Port port, node::net::ITransportHandlerPtr handler)
      : socket_(transport.ConnectTo({host, port}, this)), handler_(handler) {
  }

  ~NetTransportSocket() {
    if (socket_.IsValid()) {
      socket_.Close();
    }
  }

  // ITransportSocket

  void Send(const node::net::TransportMessage& message) override {
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
  node::net::ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportServer : public node::net::ITransportServer, public net::ISocketHandler {
 public:
  NetTransportServer(net::Transport& transport, net::Port port,
                     node::net::ITransportHandlerPtr handler)
      : server_socket_(transport.Serve(port, this)), handler_(handler) {
  }

  // ITransportServer

  void Shutdown() override {
    server_socket_.Close();
  }

  // net::ISocketHandler

  void HandleMessage(const net::Message& message,
                     net::ReplySocket back) override;

  void HandleDisconnect(const std::string& host) override {
    if (auto handler = handler_.lock()) {
      handler->HandleDisconnect(host);
    }
  }

 private:
  net::ServerSocket server_socket_;
  node::net::ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

struct NetTransport : public node::net::ITransport {
 public:
  NetTransport(net::Transport& impl, net::Port port)
      : impl_(impl), port_(port) {
  }

  const std::string& HostName() const override {
    return impl_.HostName();
  }

  node::net::ITransportServerPtr Serve(node::net::ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportServer>(impl_, port_, handler);
  }

  node::net::ITransportSocketPtr ConnectTo(const node::net::TransportAddress& host,
                                node::net::ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportSocket>(impl_, host, port_, handler);
  }

 private:
  net::Transport& impl_;
  net::Port port_;
};

}  // namespace whirl::matrix

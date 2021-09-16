#pragma once

#include <commute/transport/transport.hpp>

#include <whirl/engines/matrix/network/transport.hpp>

#include <tuple>

namespace transport = commute::transport;

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class NetTransportSocket : public transport::ISocket,
                           public net::ISocketHandler {
 public:
  NetTransportSocket(net::Transport& transport, std::string host,
                     net::Port port, transport::IHandlerPtr handler)
      : socket_(transport.ConnectTo({host, port}, this)), handler_(handler) {
  }

  ~NetTransportSocket() {
    if (socket_.IsValid()) {
      socket_.Close();
    }
  }

  // transport::ISocket

  void Send(const transport::Message& message) override {
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
  transport::IHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportServer : public transport::IServer,
                           public net::ISocketHandler {
 public:
  NetTransportServer(net::Transport& transport, net::Port port,
                     transport::IHandlerPtr handler)
      : server_socket_(transport.Serve(port, this)), handler_(handler) {
  }

  // IServer

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
  transport::IHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

struct NetTransport : public transport::ITransport {
 public:
  NetTransport(net::Transport& impl)
      : impl_(impl) {
  }

  const std::string& HostName() const override {
    return impl_.HostName();
  }

  transport::IServerPtr Serve(const std::string& port,
      transport::IHandlerPtr handler) override {
    return std::make_shared<NetTransportServer>(impl_, ParsePort(port), handler);
  }

  transport::ISocketPtr ConnectTo(
      const std::string& address,
      transport::IHandlerPtr handler) override {
    auto [host, port] = ParseAddress(address);
    return std::make_shared<NetTransportSocket>(impl_, host, port, handler);
  }

 private:
  // "{port}"
  static uint16_t ParsePort(const std::string& port_str) {
    return std::atoi(port_str.c_str());
  }

  // "{hostname}:{port}"
  static std::tuple<std::string, uint16_t> ParseAddress(const std::string& address) {
    auto pos = address.find(':');

    auto lhs = address.substr(0, pos);
    auto rhs = address.substr(pos + 1, address.length());

    return std::make_tuple(lhs, ParsePort(rhs));
  }

 private:
  net::Transport& impl_;
};

}  // namespace whirl::matrix

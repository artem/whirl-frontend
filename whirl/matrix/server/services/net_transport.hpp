#pragma once

#include <whirl/rpc/impl/net_transport.hpp>

#include <whirl/matrix/process/heap.hpp>
#include <whirl/matrix/process/network.hpp>

namespace whirl {

using namespace rpc;

//////////////////////////////////////////////////////////////////////

class NetTransportSocket : public ITransportSocket, public INetSocketHandler {
 public:
  NetTransportSocket(ProcessHeap& heap, ProcessNetwork& net, std::string peer,
                     ITransportHandlerPtr handler)
      : heap_(heap),
        socket_(net.ConnectTo(peer, this)),
        peer_(peer),
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
    return peer_;
  }

  bool IsConnected() const override {
    return socket_.IsValid();
  }

  void Close() override {
    socket_.Close();
  }

  // INetSocketHandler

  void HandleMessage(const std::string& message,
                     LightNetSocket /*back*/) override {
    auto g = heap_.Use();

    handler_->HandleMessage(message, nullptr);
  }

  void HandleDisconnect() override {
    auto g = heap_.Use();

    socket_.Close();
    handler_->HandleDisconnect();
  }

 private:
  ProcessHeap& heap_;

  ProcessSocket socket_;
  std::string peer_;
  ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportServer : public ITransportServer, public INetSocketHandler {
 public:
  NetTransportServer(ProcessHeap& heap, ProcessNetwork& net,
                     ITransportHandlerPtr handler)
      : heap_(heap), server_socket_(net.Serve(this)), handler_(handler) {
  }

  void Shutdown() override {
    // server_socket_.Close();
    // TODO
  }

  void HandleMessage(const Message& message, LightNetSocket back) override;

  void HandleDisconnect() override {
    auto g = heap_.Use();

    handler_->HandleDisconnect();
  }

 private:
  ProcessHeap& heap_;
  ProcessServerSocket server_socket_;
  ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

struct NetTransport : public ITransport {
 public:
  NetTransport(ProcessHeap& heap, ProcessNetwork& net)
      : heap_(heap), net_(net) {
  }

  ITransportServerPtr Serve(ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportServer>(heap_, net_, handler);
  }

  ITransportSocketPtr ConnectTo(const std::string& peer,
                                ITransportHandlerPtr handler) override {
    return std::make_shared<NetTransportSocket>(heap_, net_, peer, handler);
  }

 private:
  ProcessHeap& heap_;
  ProcessNetwork& net_;
};

}  // namespace whirl

#pragma once

#include <whirl/services/net_transport.hpp>

#include <whirl/matrix/process/heap.hpp>
#include <whirl/matrix/process/network.hpp>

#include <whirl/helpers/copy.hpp>

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

  void Shutdown() override {

  }

 private:
  LightNetSocket socket_;
  std::string peer_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportSocket : public INetSocketHandler {
 public:
  NetTransportSocket(ProcessHeap& heap, ProcessNetwork& net, std::string peer, ITransportHandlerPtr handler)
    : heap_(heap), socket_(net.ConnectTo(peer, this)), handler_(handler) {
  }

  void Send(const TransportMessage& message) {
    socket_.Send(message);
  }

  void Shutdown() {
    socket_.Close();
  }

  void HandleMessage(const std::string& message, LightNetSocket /*back*/) {
    auto g = heap_.Use();

    handler_->HandleMessage(MakeCopy(message), nullptr);
  }

  void HandleLostPeer() {
    auto g = heap_.Use();

    handler_->HandleLostPeer();
  }

 private:
  ProcessHeap& heap_;
  ProcessSocket socket_;
  ITransportHandlerPtr handler_;
};

//////////////////////////////////////////////////////////////////////

class NetTransportServer : public ITransportSocket, public INetSocketHandler {
 public:
  NetTransportServer(ProcessHeap& heap, ProcessNetwork& net)
    : heap_(heap), server_socket_(net.Serve(this)) {
  }

  void HandleMessage(const Message& message, LightNetSocket back) override {
    auto g = heap_.Use();

    handler_->HandleMessage(
        MakeCopy(message),
        std::make_shared<LightTransportSocket>(back, "?"));
  }

  void HandlePeerLost() override {
    auto g = heap_.Use();

    handler_->HandleLostPeer();
  }

  void Shutdown() override {
    // server_socket_.Close();
    // TODO
  }

 private:
  ProcessHeap& heap_;
  ProcessServerSocket server_socket_;
  ITransportHandlerPtr handler_;
};

}  // namespace whirl

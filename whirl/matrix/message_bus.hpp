#pragma once

#include <whirl/services/message_bus.hpp>

#include <whirl/matrix/network.hpp>
#include <whirl/matrix/process_network.hpp>

#include <whirl/matrix/runtime.hpp>
#include <whirl/matrix/allocator.hpp>

#include <whirl/matrix/log.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class MessageBusBackChannel : public IMessageBusChannel {
 public:
  MessageBusBackChannel(NetSocket&& socket, const ServerAddress& peer)
      : socket_(std::move(socket)), peer_(peer) {
  }

  const ServerAddress& Peer() const override {
    return peer_;
  }

  void Send(const Message& message) override {
    socket_.Send(message);
  }

 private:
  // Lightweight network socket
  NetSocket socket_;
  ServerAddress peer_;
};

//////////////////////////////////////////////////////////////////////

class MessageBusServer : public INetSocketHandler, public IMessageBusServer {
 public:
  MessageBusServer(ProcessNetwork& net, Heap& heap, IExecutorPtr e)
      : socket_(net.Serve(this)), heap_(heap), e_(e) {
  }

  void SetMessageHandler(ServerMessageHandler h) override {
    handler_ = h;
  }

  // Context: global
  void HandleMessage(const Message& message, NetSocket socket) override {
    HeapScope guard(&heap_);

    auto back = Back(std::move(socket));
    await::fibers::Spawn(
        [handler = handler_, message, back]() { handler(message, back); }, e_);
  }

  void HandleLost() override {
    // Lost some client, ok
  }

 private:
  IMessageBusChannelPtr Back(NetSocket&& socket) {
    // TODO: peer address
    return std::make_shared<MessageBusBackChannel>(std::move(socket), "?");
  }

 private:
  ProcessServerSocket socket_;
  Heap& heap_;
  IExecutorPtr e_;
  ServerMessageHandler handler_;
};

using MessageBusServerPtr = std::shared_ptr<MessageBusServer>;

//////////////////////////////////////////////////////////////////////

class MessageBusChannel : public IMessageBusChannel, INetSocketHandler {
 public:
  MessageBusChannel(ProcessNetwork& net, const ServerAddress& peer,
                    ClientMessageHandler handler)
      : socket_(net.ConnectTo(peer, this)), handler_(handler) {
  }

  bool IsValid() const {
    return socket_.IsValid();
  }

  const ServerAddress& Peer() const override {
    return socket_.Peer();
  }

  void Send(const Message& message) override {
    socket_.Send(message);
  }

  void HandleMessage(const Message& message, NetSocket /*socket*/) override {
    handler_(message);
  }

  void HandleLost() override {
    // Lost connection to server
  }

 private:
  IMessageBusChannelPtr Back(NetSocket&& socket) {
    return std::make_shared<MessageBusBackChannel>(std::move(socket),
                                                   socket_.Peer());
  }

 private:
  ProcessSocket socket_;
  ClientMessageHandler handler_;
};

//////////////////////////////////////////////////////////////////////

class MessageBus : public IMessageBus {
 public:
  MessageBus(ProcessNetwork& net) : net_(net) {
  }

  IMessageBusChannelPtr Dial(const ServerAddress& peer,
                             ClientMessageHandler h) override {
    auto channel = std::make_shared<MessageBusChannel>(net_, peer, h);
    if (channel->IsValid()) {
      return channel;
    } else {
      return nullptr;
    }
  }

 private:
  ProcessNetwork& net_;
};

}  // namespace whirl

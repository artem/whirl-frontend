#pragma once

#include <string>
#include <functional>
#include <memory>

namespace whirl {

// Messaging

//////////////////////////////////////////////////////////////////////

// Client channel

struct IMessageBusChannel {
  virtual ~IMessageBusChannel() = default;

  virtual const std::string& Peer() const = 0;
  virtual void Send(const std::string& message) = 0;
};

using IMessageBusChannelPtr = std::shared_ptr<IMessageBusChannel>;

//////////////////////////////////////////////////////////////////////

// Message handlers

using ServerMessageHandler =
    std::function<void(Message, IMessageBusChannelPtr)>;

using ClientMessageHandler = std::function<void(Message)>;

//////////////////////////////////////////////////////////////////////

// Server

struct IMessageBusServer {
  using Message = std::string;

  virtual ~IMessageBusServer() = default;

  virtual void SetMessageHandler(ServerMessageHandler handler) = 0;
  // virtual void HandleMessage(const Message& message) = 0;
};

using IMessageBusServerPtr = std::shared_ptr<IMessageBusServer>;

//////////////////////////////////////////////////////////////////////

// Connector

struct IMessageBus {
  virtual ~IMessageBus() = default;
  virtual IMessageBusChannelPtr Dial(const std::string& peer,
                                     ClientMessageHandler handler) = 0;
};

using IMessageBusPtr = std::shared_ptr<IMessageBus>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

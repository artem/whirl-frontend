#pragma once

#include <string>
#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using TransportMessage = std::string;

using TransportAddress = std::string;

//////////////////////////////////////////////////////////////////////

struct ITransportSocket {
  virtual ~ITransportSocket() = default;

  virtual const std::string& Peer() const = 0;
  virtual void Send(const TransportMessage& message) = 0;
  virtual void Shutdown() = 0;
};

using ITransportSocketPtr = std::shared_ptr<ITransportSocket>;

//////////////////////////////////////////////////////////////////////

struct ITransportHandler {
  virtual ~ITransportHandler() = default;

  virtual void HandleMessage(const TransportMessage& message, ITransportSocketPtr back) = 0;
  virtual void HandleLostPeer() = 0;
};

using ITransportHandlerPtr = std::shared_ptr<ITransportHandler>;

//////////////////////////////////////////////////////////////////////

struct ITransportServer {
  virtual ~ITransportServer() = default;

  virtual void Shutdown() = 0;
};

using ITransportServerPtr = std::shared_ptr<ITransportServer>;

//////////////////////////////////////////////////////////////////////

struct ITransport {
  virtual ~ITransport() = default;

  virtual ITransportServerPtr Serve(ITransportHandlerPtr handler) = 0;
  virtual ITransportSocketPtr ConnectTo(const std::string& peer, ITransportHandlerPtr handler) = 0;
};

using ITransportPtr = std::shared_ptr<ITransport>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

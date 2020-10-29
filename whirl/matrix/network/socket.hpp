#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/link.hpp>

namespace whirl {

class Network;

// Sockets

//////////////////////////////////////////////////////////////////////

// Client socket

struct NetSocket {
 public:
  NetSocket(Network* net, Link* link, NetEndpointId self, NetEndpointId peer);
  ~NetSocket();

  static NetSocket Invalid();

  // Non-copyable
  NetSocket(const NetSocket& that) = delete;
  NetSocket& operator=(const NetSocket& that) = delete;

  // Movable
  NetSocket(NetSocket&& that);

  const std::string& Peer() const {
    return link_->End();
  }

  bool IsValid() const;
  void Send(const Message& message);
  void Close();

 private:
  NetPacket MakePacket(const Message& message);
  void Invalidate();

 private:
  NetEndpointId self_;
  NetEndpointId peer_;
  Network* net_;
  Link* link_;
};

//////////////////////////////////////////////////////////////////////

// Server socket

class NetServerSocket {
 public:
  NetServerSocket(Network* net, ServerAddress self);
  ~NetServerSocket();

  // Non-copyable
  NetServerSocket(const NetServerSocket& that) = delete;
  NetServerSocket operator=(const NetServerSocket& that) = delete;

  NetServerSocket(NetServerSocket&& that);

 private:
  ServerAddress self_;
  Network* net_;
};

//////////////////////////////////////////////////////////////////////

// Just throws messages to network, does not own net endpoint

class LightNetSocket {
 public:
  LightNetSocket(Link* link, NetEndpointId self, NetEndpointId peer);

  void Send(const Message& message);

  const std::string& Peer() const {
    return link_->End();
  }

 private:
  NetPacket MakePacket(const Message& message);

 private:
  NetEndpointId self_;
  NetEndpointId peer_;
  Link* link_;
};

//////////////////////////////////////////////////////////////////////

struct INetSocketHandler {
  virtual ~INetSocketHandler() = default;

  virtual void HandleMessage(const Message& message, LightNetSocket back) = 0;

  virtual void HandleDisconnect() = 0;
};

}  // namespace whirl

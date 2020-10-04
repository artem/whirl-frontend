#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>

namespace whirl {

// TODO: socket = 2 uni-directional socket channels

class Network;

// Sockets

//////////////////////////////////////////////////////////////////////

// Client socket

struct NetSocket {
 public:
  NetSocket(Network* net, NetEndpointId self, NetEndpointId peer);
  ~NetSocket();

  static NetSocket Invalid();

  // Non-copyable
  NetSocket(const NetSocket& that) = delete;
  NetSocket& operator=(const NetSocket& that) = delete;

  // Movable
  NetSocket(NetSocket&& that);

  bool IsValid() const;
  void Send(const Message& message);
  void Close();

 private:
  void Invalidate();

 private:
  NetEndpointId self_;
  NetEndpointId peer_;
  Network* net_;
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
  LightNetSocket(Network* net, NetEndpointId self, NetEndpointId peer);

  void Send(const Message& message);

 private:
  NetEndpointId self_;
  NetEndpointId peer_;
  Network* net_;
};

//////////////////////////////////////////////////////////////////////

struct INetSocketHandler {
  virtual ~INetSocketHandler() = default;

  virtual void HandleMessage(const Message& message, LightNetSocket back) = 0;

  // Peer disconnected
  virtual void HandlePeerLost() = 0;
};

}  // namespace whirl

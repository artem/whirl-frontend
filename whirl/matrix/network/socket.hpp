#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>
#include <whirl/matrix/network/message.hpp>

namespace whirl {

// TODO: socket = 2 uni-directional socket channels

//////////////////////////////////////////////////////////////////////

class Network;

// Sockets

struct NetSocket {
 public:
  NetSocket(Network* net, NetEndpointId self, NetEndpointId peer,
            bool client = false);
  ~NetSocket();

  static NetSocket Invalid() {
    return NetSocket{nullptr, 0, 0};
  }

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
  bool client_;
};

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

struct INetSocketHandler {
  virtual ~INetSocketHandler() = default;

  virtual void HandleMessage(const Message& message, NetSocket back) = 0;

  // Peer disconnected
  virtual void HandleLost() = 0;
};

}  // namespace whirl

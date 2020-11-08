#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/timestamp.hpp>
#include <whirl/matrix/network/packet.hpp>

#include <memory>

namespace whirl::net {

class Transport;
class Link;

// Sockets

//////////////////////////////////////////////////////////////////////

class ClientSocket {
 public:
  ClientSocket(Transport* transport, Link* link, Port self, Port server,
               Timestamp ts);
  ~ClientSocket();

  const std::string& Peer() const;

  bool IsValid() const;
  void Send(const Message& message);
  void Close();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

//////////////////////////////////////////////////////////////////////

class ServerSocket {
 public:
  ServerSocket(Transport* transport, Port port);
  ~ServerSocket();

  void Close();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

//////////////////////////////////////////////////////////////////////

// Just throws messages to network, does not own net endpoint

class ReplySocket {
 public:
  ReplySocket(const Packet& packet, Link* out);

  void Send(const Message& message);

  const std::string& Peer() const;

 private:
  Packet MakePacket(const Message& message);

 private:
  Link* link_;
  Port self_port_;
  Port peer_port_;
  Timestamp ts_;
};

//////////////////////////////////////////////////////////////////////

struct ISocketHandler {
  virtual ~ISocketHandler() = default;

  virtual void HandleMessage(const Message& message, ReplySocket back) = 0;

  virtual void HandleDisconnect(const std::string& peer) = 0;
};

}  // namespace whirl::net

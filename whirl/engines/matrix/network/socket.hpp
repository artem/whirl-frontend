#pragma once

#include <whirl/engines/matrix/network/address.hpp>
#include <whirl/engines/matrix/network/message.hpp>
#include <whirl/engines/matrix/network/timestamp.hpp>
#include <whirl/engines/matrix/network/packet.hpp>

#include <memory>

namespace whirl::matrix::net {

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

// Just throws messages to network, does not own network endpoint

class ReplySocket {
 public:
  ReplySocket(const Packet::Header& incoming, Link* out);

  void Send(const Message& message);

  const std::string& Peer() const;

 private:
  Packet MakePacket(const Message& message) const;
  Packet::Header MakeHeader(const Message& message) const;

 private:
  Link* link_;
  Port self_port_;
  Port peer_port_;
  Timestamp ts_;
};

}  // namespace whirl::matrix::net

#pragma once

#include <whirl/engines/matrix/network/packet.hpp>

#include <string>

namespace whirl::net {

class Link;

struct INetServer {
  virtual ~INetServer() = default;

  virtual const std::string& HostName() const = 0;

  virtual void HandlePacket(const Packet& packet, Link* out) = 0;
};

}  // namespace whirl::net

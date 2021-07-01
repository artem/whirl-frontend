#pragma once

#include <whirl/engines/matrix/network/packet.hpp>
#include <whirl/engines/matrix/network/zone.hpp>

#include <string>

namespace whirl::matrix::net {

class Link;

struct IServer {
  virtual ~IServer() = default;

  virtual const std::string& HostName() const = 0;
  virtual ZoneId Zone() const = 0;

  virtual void HandlePacket(const Packet& packet, Link* out) = 0;
};

}  // namespace whirl::matrix::net

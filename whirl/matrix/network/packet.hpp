#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/timestamp.hpp>

namespace whirl::net {

enum class EPacketType { Data, Reset, Ping };

struct Packet {
  EPacketType type;
  Port source_port;
  Message message;
  Port dest_port;
  Timestamp ts;
};

}  // namespace whirl::net

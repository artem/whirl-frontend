#pragma once

#include <whirl/engines/matrix/network/address.hpp>
#include <whirl/engines/matrix/network/message.hpp>
#include <whirl/engines/matrix/network/timestamp.hpp>

namespace whirl::net {

enum class EPacketType { Data, Reset, Ping };

struct Packet {
  struct Header {
    EPacketType type;
    Port source_port;
    Port dest_port;
    Timestamp ts;
  };

  Header header;
  Message message;
};

}  // namespace whirl::net

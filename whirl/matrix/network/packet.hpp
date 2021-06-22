#pragma once

#include <whirl/matrix/network/address.hpp>
#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/timestamp.hpp>

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

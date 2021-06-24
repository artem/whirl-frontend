#pragma once

#include <whirl/engines/matrix/network/address.hpp>
#include <whirl/engines/matrix/network/message.hpp>
#include <whirl/engines/matrix/network/timestamp.hpp>

namespace whirl::matrix::net {

struct Packet {
  enum class Type {
    Data,   // User message
    Reset,  // Connection reset by peer
    Ping    // Keep-alive
  };

  struct Header {
    Type type;
    Port source_port;
    Port dest_port;
    Timestamp ts;
  };

  Header header;
  Message message;
};

}  // namespace whirl::matrix::net

#pragma once

#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/network/endpoint_id.hpp>

namespace whirl {

enum EPacketType { Data, Reset };

struct NetPacket {
  EPacketType type;
  NetEndpointId source;
  Message message;
  NetEndpointId dest;

  bool IsData() const {
    return type == EPacketType::Data;
  }
};

}  // namespace whirl

#pragma once

#include <cstdlib>

#include <whirl/time.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Clocks

inline int InitClockDrift() {
  return GetWorldBehaviour()->InitClockDrift();
}

inline TimePoint GlobalStartTime() {
  return GetWorldBehaviour()->GlobalStartTime();
}

inline TimePoint ResetMonotonicClock() {
  return GetWorldBehaviour()->ResetMonotonicClock();
}

inline TimePoint InitLocalClockOffset() {
  return GetWorldBehaviour()->InitLocalClockOffset();
}

inline Duration TrueTimeUncertainty() {
  return GetWorldBehaviour()->TrueTimeUncertainty();
}

// Network

inline TimePoint NetPacketDeliveryTime(const net::Packet& packet) {
  return GetWorldBehaviour()->NetPacketDeliveryTime(packet);
}

inline bool DuplicateNetPacket() {
  return GlobalRandomNumber(3) == 0;
}

}  // namespace whirl

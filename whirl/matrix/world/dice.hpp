#pragma once

#include <cstdlib>

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Clocks

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

inline TimePoint NetPacketDeliveryTime() {
  return GetWorldBehaviour()->NetPacketDeliveryTime();
}

inline bool DuplicateNetPacket() {
  return GlobalRandomNumber(3) == 0;
}

}  // namespace whirl

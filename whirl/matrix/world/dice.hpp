#pragma once

#include <cstdlib>

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Local clocks

inline TimePoint ResetMonotonicClock() {
  return GlobalRandomNumber(1, 100);
}

inline TimePoint InitLocalClockOffset() {
  return GlobalRandomNumber(100);
}

// Network

inline TimePoint NetPacketDeliveryTime() {
  return GlobalRandomNumber(30, 60);
}

inline bool DuplicateNetPacket() {
  return GlobalRandomNumber(3) == 0;
}

}  // namespace params

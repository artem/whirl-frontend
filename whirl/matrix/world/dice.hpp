#pragma once

#include <cstdlib>

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Clocks

inline TimePoint ResetMonotonicClock() {
  return GlobalRandomNumber(1, 100);
}

inline TimePoint InitLocalClockOffset() {
  return GlobalRandomNumber(1000);
}

inline Duration TrueTimeUncertainty() {
  return GlobalRandomNumber(5, 500);
}

// Network

inline TimePoint NetPacketDeliveryTime() {
  if (GlobalRandomNumber() % 5 == 0) {
    return GlobalRandomNumber(10, 1000);
  }
  return GlobalRandomNumber(30, 60);
}

inline bool DuplicateNetPacket() {
  return GlobalRandomNumber(3) == 0;
}

}  // namespace whirl

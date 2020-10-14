#pragma once

#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Asynchronous world

class CrazyWorldBehaviour : public IWorldBehaviour {
  TimePoint GlobalStartTime() override {
    return GlobalRandomNumber(1000);
  }

  TimePoint ResetMonotonicClock() override {
    return GlobalRandomNumber(1, 100);
  }

  TimePoint InitLocalClockOffset() override {
    return GlobalRandomNumber(1000);
  }

  Duration TrueTimeUncertainty() override {
    return GlobalRandomNumber(5, 500);
  }

// Network

  TimePoint NetPacketDeliveryTime() override {
    if (GlobalRandomNumber() % 5 == 0) {
      return GlobalRandomNumber(10, 1000);
    }
    return GlobalRandomNumber(30, 60);
  }
};

}  // namespace whirl

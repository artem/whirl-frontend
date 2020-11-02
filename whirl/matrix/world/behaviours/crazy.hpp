#pragma once

#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

// Asynchronous world

class CrazyWorldBehaviour : public IWorldBehaviour {
  // Time

  // [-75, +75]
  int InitClockDrift() override {
    if (GlobalRandomNumber() % 3 == 0) {
      // Super-fast monotonic clocks
      // x3-x4 faster than global time
      return 200 + GlobalRandomNumber(100);
    } else if (GlobalRandomNumber() % 2 == 0) {
      // Relatively fast
      return 75 + GlobalRandomNumber(25);
    } else {
      // Relatively slow
      return -75 + (int)GlobalRandomNumber(25 + 1);
    }
    // return -75 + (int)GlobalRandomNumber(75 * 2 + 1);
  }

  int ClockDriftBound() override {
    return 300;
  }

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

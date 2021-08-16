#include <whirl/engines/matrix/world/time_models/crazy.hpp>

#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix {

class CrazyTimeModel : public ITimeModel {
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

  TimePoint InitWallClockOffset() override {
    return GlobalRandomNumber(1000);
  }

  Duration TrueTimeUncertainty() override {
    return GlobalRandomNumber(5, 500);
  }

  Duration DiskWrite(size_t /*bytes*/) override {
    return GlobalRandomNumber(10, 250);
  }

  Duration DiskRead(size_t /*bytes*/) override {
    return GlobalRandomNumber(10, 50);
  }

  // Network

  TimePoint FlightTime(const net::IServer* /*start*/,
                       const net::IServer* /*end*/,
                       const net::Packet& packet) override {
    if (packet.header.type != net::Packet::Type::Data) {
      // Service packet, do not affect randomness
      return 50;
    }

    if (GlobalRandomNumber() % 5 == 0) {
      return GlobalRandomNumber(10, 1000);
    }
    return GlobalRandomNumber(30, 60);
  }

  // Threads

  Duration ThreadPause() override {
    return GlobalRandomNumber(5, 50);
  }
};

ITimeModelPtr MakeCrazyTimeModel() {
  return std::make_shared<CrazyTimeModel>();
}

}  // namespace whirl::matrix

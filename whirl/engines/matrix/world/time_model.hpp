#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/network/packet.hpp>
#include <whirl/engines/matrix/network/server.hpp>

#include <memory>

namespace whirl::matrix {

struct ITimeModel {
  virtual ~ITimeModel() = default;

  virtual TimePoint GlobalStartTime() = 0;

  // Clocks

  virtual int InitClockDrift() = 0;
  virtual int ClockDriftBound() = 0;

  // Monotonic clock

  virtual TimePoint ResetMonotonicClock() = 0;

  // Wall clock

  virtual TimePoint InitWallClockOffset() = 0;

  // TrueTime

  virtual Duration TrueTimeUncertainty() = 0;

  // Disk

  virtual Duration DiskWrite(size_t bytes) = 0;
  virtual Duration DiskRead(size_t bytes) = 0;

  // Network

  // DPI =)
  virtual Duration FlightTime(const net::IServer* start,
                              const net::IServer* end,
                              const net::Packet& packet) = 0;

  // Threads

  virtual Duration ThreadPause() = 0;
};

using ITimeModelPtr = std::shared_ptr<ITimeModel>;

}  // namespace whirl::matrix

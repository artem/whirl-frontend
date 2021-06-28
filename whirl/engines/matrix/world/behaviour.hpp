#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/network/packet.hpp>
#include <whirl/engines/matrix/network/server.hpp>

#include <memory>

namespace whirl::matrix {

struct IWorldBehaviour {
  virtual ~IWorldBehaviour() = default;

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

  virtual Duration DiskWrite() = 0;
  virtual Duration DiskRead() = 0;

  // Network

  // DPI =)
  virtual Duration FlightTime(const net::IServer* start,
                              const net::IServer* end,
                              const net::Packet& packet) = 0;

  // Threads

  virtual Duration ThreadPause() = 0;
};

using IWorldBehaviourPtr = std::shared_ptr<IWorldBehaviour>;

}  // namespace whirl::matrix

#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/network/packet.hpp>

#include <memory>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct IWorldBehaviour {
  virtual ~IWorldBehaviour() = default;

  // Time

  virtual int InitClockDrift() = 0;
  virtual int ClockDriftBound() = 0;

  virtual TimePoint GlobalStartTime() = 0;

  virtual TimePoint ResetMonotonicClock() = 0;

  virtual TimePoint InitLocalClockOffset() = 0;

  virtual Duration TrueTimeUncertainty() = 0;

  virtual Duration DiskWrite() = 0;
  virtual Duration DiskRead() = 0;

  // Network

  // DPI =)
  virtual Duration FlightTime(const net::Packet& packet) = 0;
};

using IWorldBehaviourPtr = std::shared_ptr<IWorldBehaviour>;

//////////////////////////////////////////////////////////////////////

IWorldBehaviourPtr DefaultBehaviour();

}  // namespace whirl::matrix

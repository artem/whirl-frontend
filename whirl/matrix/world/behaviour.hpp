#pragma once

#include <whirl/time.hpp>

#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

struct IWorldBehaviour {
  virtual ~IWorldBehaviour() = default;

  // Time

  virtual TimePoint GlobalStartTime() = 0;

  virtual TimePoint ResetMonotonicClock() = 0;

  virtual TimePoint InitLocalClockOffset() = 0;

  virtual Duration TrueTimeUncertainty() = 0;

  // Network

  virtual Duration NetPacketDeliveryTime() = 0;
};

using IWorldBehaviourPtr = std::shared_ptr<IWorldBehaviour>;

//////////////////////////////////////////////////////////////////////

IWorldBehaviourPtr DefaultBehaviour();

}  // namespace whirl

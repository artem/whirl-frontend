#pragma once

#include <whirl/time.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/world/behaviour.hpp>

#include <cstdlib>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Deterministic randomness

using RandomUInt = size_t;

RandomUInt GlobalRandomNumber();

RandomUInt GlobalRandomNumber(size_t bound);

// [lo, hi)
RandomUInt GlobalRandomNumber(size_t lo, size_t hi);

//////////////////////////////////////////////////////////////////////

// Time

TimePoint GlobalNow();

//////////////////////////////////////////////////////////////////////

// Behaviour

IWorldBehaviourPtr GetWorldBehaviour();

//////////////////////////////////////////////////////////////////////

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

}  // namespace whirl

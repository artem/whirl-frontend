#pragma once

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/world/clock.hpp>

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

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

}  // namespace whirl

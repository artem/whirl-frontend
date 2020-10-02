#pragma once

#include <whirl/matrix/time.hpp>
#include <whirl/matrix/clock.hpp>

#include <cstdlib>

namespace whirl {

// Requirement: call from IActor::MakeStep only

//////////////////////////////////////////////////////////////////////

// Random

using RandomUInt = size_t;

RandomUInt GlobalRandomNumber();

//////////////////////////////////////////////////////////////////////

// Time

const WorldClock& GetWorldClock();
TimePoint GlobalNow();

//////////////////////////////////////////////////////////////////////

// For logging

size_t WorldStep();
bool AmIActor();
std::string CurrentActorName();

}  // namespace whirl

#pragma once

#include <whirl/time.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/history/recorder.hpp>

#include <cstdlib>

namespace whirl {

// Global world services used by different components of simulation

//////////////////////////////////////////////////////////////////////

// Deterministic randomness

using RandomUInt = size_t;

RandomUInt GlobalRandomNumber();

RandomUInt GlobalRandomNumber(size_t bound);

// [lo, hi)
RandomUInt GlobalRandomNumber(size_t lo, size_t hi);

//////////////////////////////////////////////////////////////////////

// Global time

TimePoint GlobalNow();

//////////////////////////////////////////////////////////////////////

// World behaviour

IWorldBehaviourPtr GetWorldBehaviour();

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder();

//////////////////////////////////////////////////////////////////////

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

}  // namespace whirl

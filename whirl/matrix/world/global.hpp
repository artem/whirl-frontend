#pragma once

#include <whirl/time.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/history/recorder.hpp>
#include <whirl/matrix/log/log.hpp>

#include <cstdlib>
#include <vector>
#include <string>

namespace whirl {

// Global world services used by different components of simulation

//////////////////////////////////////////////////////////////////////

size_t WorldSeed();

// Varies by simulation (seed)
// Requirement: `randomizer` > 0
size_t ThisWorldConst(size_t randomizer);

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

std::vector<std::string> GetClusterAddresses();

//////////////////////////////////////////////////////////////////////

// For adversary

size_t ServerCount();

//////////////////////////////////////////////////////////////////////

Log& GetLog();

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

}  // namespace whirl

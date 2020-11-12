#pragma once

#include <whirl/matrix/world/global/random.hpp>
#include <whirl/matrix/world/global/time.hpp>

#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/history/recorder.hpp>
#include <whirl/matrix/log/log.hpp>

#include <cstdlib>
#include <vector>
#include <string>
#include <any>

namespace whirl {

// Global world services used by different components of simulation

//////////////////////////////////////////////////////////////////////

size_t WorldSeed();

// Varies by simulation (seed)
// Requirement: `randomizer` > 0
size_t ThisWorldConst(size_t randomizer);

//////////////////////////////////////////////////////////////////////

// World behaviour

IWorldBehaviourPtr GetWorldBehaviour();

bool IsThereAdversary();

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder();

//////////////////////////////////////////////////////////////////////

size_t GetClusterSize();

std::vector<std::string> GetCluster();

//////////////////////////////////////////////////////////////////////

Log& GetLog();

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

//////////////////////////////////////////////////////////////////////

// Global user-set variables

namespace detail {

std::any GetGlobal(const std::string& name);

}  // namespace detail

template <typename T>
T GetGlobal(const std::string& name) {
  return std::any_cast<T>(detail::GetGlobal(name));
}

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

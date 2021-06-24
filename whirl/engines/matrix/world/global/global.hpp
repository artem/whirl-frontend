#pragma once

#include <whirl/engines/matrix/world/global/random.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/vars.hpp>

#include <whirl/engines/matrix/world/behaviour.hpp>
#include <whirl/engines/matrix/history/recorder.hpp>
#include <whirl/engines/matrix/log/backend.hpp>

#include <cstdlib>
#include <vector>
#include <string>
#include <any>

namespace whirl::matrix {

// Global world services used by different components of simulation

//////////////////////////////////////////////////////////////////////

size_t WorldSeed();

//////////////////////////////////////////////////////////////////////

// World behaviour

IWorldBehaviourPtr GetWorldBehaviour();

bool IsThereAdversary();

//////////////////////////////////////////////////////////////////////

HistoryRecorder& GetHistoryRecorder();

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetPool(const std::string& name);

//////////////////////////////////////////////////////////////////////

LogBackend& GetLog();

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::matrix

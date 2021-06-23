#pragma once

#include <whirl/matrix/world/global/random.hpp>
#include <whirl/matrix/world/global/time.hpp>
#include <whirl/matrix/world/global/vars.hpp>

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

//////////////////////////////////////////////////////////////////////

// World behaviour

IWorldBehaviourPtr GetWorldBehaviour();

bool IsThereAdversary();

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder();

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetPool(const std::string& name);

//////////////////////////////////////////////////////////////////////

Log& GetLog();

// For logging

size_t WorldStepNumber();
bool AmIActor();
std::string CurrentActorName();

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

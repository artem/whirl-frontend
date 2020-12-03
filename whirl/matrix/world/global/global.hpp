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
void SetGlobal(const std::string& name, std::any value);

}  // namespace detail

template <typename T>
T GetGlobal(const std::string& name) {
  return std::any_cast<T>(detail::GetGlobal(name));
}

template <typename T>
void SetGlobal(const std::string& name, T value) {
  detail::SetGlobal(name, std::any{std::move(value)});
}

template <typename T>
class GlobalVar {
 public:
  GlobalVar(const std::string name)
    : name_(std::move(name)) {
  }

  void Set(T value) {
    SetGlobal(name_, std::move(value));
  }

  T Get() const {
    return GetGlobal<T>(name_);
  }

 private:
  std::string name_;
};

class GlobalCounter : protected GlobalVar<size_t> {
 public:
  GlobalCounter(std::string name)
    : GlobalVar(name) {
  }

  size_t Increment() {
    size_t old = Get();
    Set(old + 1);
    return old + 1;
  }

  size_t Get() const {
    return GlobalVar::Get();
  }
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

#include <whirl/matrix/world/global/global.hpp>

#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

size_t WorldSeed() {
  return WorldImpl::Access()->Seed();
}

size_t ThisWorldConst(size_t randomizer) {
  return WorldSeed() % randomizer;
}

//////////////////////////////////////////////////////////////////////

RandomUInt GlobalRandomNumber() {
  return WorldImpl::Access()->RandomNumber();
}

//////////////////////////////////////////////////////////////////////

TimePoint GlobalNow() {
  return WorldImpl::Access()->Now();
}

//////////////////////////////////////////////////////////////////////

// Behaviour

IWorldBehaviourPtr GetWorldBehaviour() {
  return WorldImpl::Access()->Behaviour();
}

bool IsThereAdversary() {
  return WorldImpl::Access()->HasAdversary();
}

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder() {
  return WorldImpl::Access()->HistoryRecorder();
}

//////////////////////////////////////////////////////////////////////

size_t GetClusterSize() {
  return WorldImpl::Access()->ClusterSize();
}

std::vector<std::string> GetCluster() {
  return WorldImpl::Access()->GetCluster();
}

//////////////////////////////////////////////////////////////////////

Log& GetLog() {
  return WorldImpl::Access()->GetLog();
}

size_t WorldStepNumber() {
  return WorldImpl::Access()->CurrentStep();
}

bool AmIActor() {
  return WorldImpl::Access()->CurrentActor() != nullptr;
}

std::string CurrentActorName() {
  return WorldImpl::Access()->CurrentActor()->Name();
}

//////////////////////////////////////////////////////////////////////

namespace detail {

std::any GetGlobal(const std::string& name) {
  return WorldImpl::Access()->GetGlobal(name);
}

void SetGlobal(const std::string& name, std::any value) {
  return WorldImpl::Access()->SetGlobal(name, std::move(value));
}

}  // namespace detail

}  // namespace whirl

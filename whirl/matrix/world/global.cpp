#include <whirl/matrix/world/global.hpp>

#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

RandomUInt GlobalRandomNumber() {
  return WorldImpl::Access()->RandomNumber();
}

RandomUInt GlobalRandomNumber(size_t bound) {
  WHEELS_VERIFY(bound > 0, "bound = 0");
  return GlobalRandomNumber() % bound;
}

RandomUInt GlobalRandomNumber(size_t lo, size_t hi) {
  WHEELS_VERIFY(lo < hi, "Invalid range");
  return lo + GlobalRandomNumber(hi - lo);
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

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder() {
  return WorldImpl::Access()->HistoryRecorder();
}

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetClusterAddresses() {
  return WorldImpl::Access()->ClusterAddresses();
}

//////////////////////////////////////////////////////////////////////

// For adversary

size_t ServerCount() {
  return WorldImpl::Access()->NumServers();
}

std::string GetServerName(size_t index) {
  return WorldImpl::Access()->GetServer(index).Name();
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

}  // namespace whirl

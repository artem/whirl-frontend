#include <whirl/engines/matrix/world/global/global.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

size_t WorldSeed() {
  return WorldImpl::Access()->Seed();
}

//////////////////////////////////////////////////////////////////////

size_t StepRandomNumber() {
  return WorldImpl::Access()->StepRandomNumber();
}

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

std::vector<std::string> GetPool(const std::string& name) {
  return WorldImpl::Access()->GetPool(name);
}

//////////////////////////////////////////////////////////////////////

LogBackend& GetLog() {
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

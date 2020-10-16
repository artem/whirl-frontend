#include <whirl/matrix/world/global.hpp>

#include <whirl/matrix/world/world.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

RandomUInt GlobalRandomNumber() {
  return World::Access()->RandomNumber();
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
  return World::Access()->Now();
}

//////////////////////////////////////////////////////////////////////

// Behaviour

IWorldBehaviourPtr GetWorldBehaviour() {
  return World::Access()->Behaviour();
}

//////////////////////////////////////////////////////////////////////

histories::Recorder& GetHistoryRecorder() {
  return World::Access()->HistoryRecorder();
}

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetClusterAddresses() {
  return World::Access()->ClusterAddresses();
}

//////////////////////////////////////////////////////////////////////

size_t WorldStepNumber() {
  return World::Access()->CurrentStep();
}

bool AmIActor() {
  return World::Access()->CurrentActor() != nullptr;
}

std::string CurrentActorName() {
  return World::Access()->CurrentActor()->Name();
}

}  // namespace whirl

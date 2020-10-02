#include <whirl/matrix/global.hpp>

#include <whirl/matrix/world.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

RandomUInt GlobalRandomNumber() {
  return World::Access()->RandomNumber();
}

//////////////////////////////////////////////////////////////////////

const WorldClock& GetWorldClock() {
  return World::Access()->Clock();
}

TimePoint GlobalNow() {
  return World::Access()->Now();
}

//////////////////////////////////////////////////////////////////////

size_t WorldStep() {
  return World::Access()->CurrentStep();
}

bool AmIActor() {
  return World::Access()->CurrentActor() != nullptr;
}

std::string CurrentActorName() {
  return World::Access()->CurrentActor()->Name();
}

}  // namespace whirl

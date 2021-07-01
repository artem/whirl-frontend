#include <whirl/engines/matrix/world/global/global.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl::matrix {

static WorldImpl* ThisWorld() {
  return WorldImpl::Access();
}

//////////////////////////////////////////////////////////////////////

size_t WorldSeed() {
  return ThisWorld()->Seed();
}

//////////////////////////////////////////////////////////////////////

size_t StepRandomNumber() {
  return ThisWorld()->StepRandomNumber();
}

RandomUInt GlobalRandomNumber() {
  return ThisWorld()->RandomNumber();
}

//////////////////////////////////////////////////////////////////////

TimePoint GlobalNow() {
  return ThisWorld()->Now();
}

//////////////////////////////////////////////////////////////////////

// Behaviour

ITimeModelPtr GetTimeModel() {
  return ThisWorld()->TimeModel();
}

bool IsThereAdversary() {
  return ThisWorld()->HasAdversary();
}

//////////////////////////////////////////////////////////////////////

HistoryRecorder& GetHistoryRecorder() {
  return ThisWorld()->GetHistoryRecorder();
}

//////////////////////////////////////////////////////////////////////

std::vector<std::string> GetPool(const std::string& name) {
  return ThisWorld()->GetPool(name);
}

//////////////////////////////////////////////////////////////////////

size_t WorldStepNumber() {
  return ThisWorld()->CurrentStep();
}

bool AmIActor() {
  return ThisWorld()->CurrentActor() != nullptr;
}

IActor* CurrentActor() {
  return ThisWorld()->CurrentActor();
}

//////////////////////////////////////////////////////////////////////

Guid GenerateGuid() {
  return ThisWorld()->GenerateGuid();
}

//////////////////////////////////////////////////////////////////////

namespace detail {

std::any GetGlobal(const std::string& name) {
  return ThisWorld()->GetGlobal(name);
}

void SetGlobal(const std::string& name, std::any value) {
  return ThisWorld()->SetGlobal(name, std::move(value));
}

}  // namespace detail

}  // namespace whirl::matrix

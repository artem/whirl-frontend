#include <whirl/engines/matrix/world/world.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl::matrix {

World::World(size_t seed) : impl_(std::make_unique<WorldImpl>(seed)) {
}

World::~World() {
}

size_t World::Seed() const {
  return impl_->Seed();
}

void World::AddServer(INodeFactoryPtr node) {
  impl_->AddServer(node);
}

void World::AddServers(size_t count, INodeFactoryPtr node) {
  for (size_t i = 0; i < count; ++i) {
    AddServer(node);
  }
}

void World::AddClient(INodeFactoryPtr node) {
  impl_->AddClient(std::move(node));
}

void World::AddClients(size_t count, INodeFactoryPtr node) {
  for (size_t i = 0; i < count; ++i) {
    AddClient(node);
  }
}

void World::SetTimeModel(ITimeModelPtr time_model) {
  impl_->SetTimeModel(std::move(time_model));
}

void World::SetAdversary(adversary::Strategy strategy) {
  impl_->SetAdversary(std::move(strategy));
}

void World::Start() {
  impl_->Start();
}

bool World::Step() {
  return impl_->Step();
}

void World::MakeSteps(size_t count) {
  impl_->MakeSteps(count);
}

void World::RestartServer(size_t index) {
  impl_->RestartServer(index);
}

size_t World::Stop() {
  return impl_->Stop();
}

size_t World::Digest() const {
  return impl_->Digest();
}

const EventLog& World::EventLog() const {
  return impl_->GetLog().GetEvents();
}

const histories::History& World::History() const {
  return impl_->History();
}

size_t World::StepCount() const {
  return impl_->CurrentStep();
}

Duration World::TimeElapsed() const {
  return impl_->TimeElapsed();
}

void World::SetGlobalImpl(const std::string& key, std::any value) {
  impl_->SetGlobal(key, value);
}

std::any World::GetGlobalImpl(const std::string& key) const {
  return impl_->GetGlobal(key);
}

}  // namespace whirl::matrix

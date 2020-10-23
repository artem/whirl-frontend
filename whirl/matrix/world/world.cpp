#include <whirl/matrix/world/world.hpp>

#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

World::World(size_t seed) : impl_(std::make_unique<WorldImpl>(seed)) {
}

World::~World() {
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

void World::SetBehaviour(IWorldBehaviourPtr behaviour) {
  impl_->SetBehaviour(std::move(behaviour));
}

void World::SetAdversary(adversary::Strategy strategy) {
  impl_->SetAdversary(std::move(strategy));
}

void World::WriteLogTo(std::ostream& out) {
  impl_->GetLog().SetOutput(&out);
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

void World::Stop() {
  impl_->Stop();
}

size_t World::NumCompletedCalls() const {
  return impl_->HistoryRecorder().NumCompletedCalls();
}

const histories::History& World::History() const {
  return impl_->History();
}

}  // namespace whirl

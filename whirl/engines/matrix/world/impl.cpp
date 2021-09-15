#include <whirl/engines/matrix/world/impl.hpp>

#include <whirl/engines/matrix/world/time_models/crazy.hpp>

#include <timber/log.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

static WorldImpl* this_world = nullptr;

WorldImpl::WorldGuard::WorldGuard(WorldImpl* world) {
  this_world = world;
}

WorldImpl::WorldGuard::~WorldGuard() {
  this_world = nullptr;
}

WorldImpl* WorldImpl::Access() {
  WHEELS_VERIFY(this_world != nullptr, "Not in world context");
  return this_world;
}

//////////////////////////////////////////////////////////////////////

ITimeModelPtr WorldImpl::DefaultTimeModel() {
  return MakeCrazyTimeModel();
}

void WorldImpl::Start() {
  WorldGuard g(this);

  LOG_INFO("Seed: {}", seed_);

  SetStartTime();
  start_time_ = time_.Now();

  // Start network:
  AddActor(&network_);
  Scope(network_)->Start();

  LOG_INFO("Cluster: ?, clients: {}", clients_.size());

  LOG_INFO("Starting cluster...");

  // Start servers
  for (auto& [_, pool] : pools_) {
    for (auto& server : pool) {
      Scope(server)->Start();
    }
  }

  LOG_INFO("Starting clients...");

  // Start clients
  for (auto& client : clients_) {
    Scope(client)->Start();
  }

  // Start adversary
  if (!adversaries_.empty()) {
    LOG_INFO("Starting adversary...");
    Scope(adversaries_.front())->Start();
  }

  LOG_INFO("World started");
}

bool WorldImpl::Step() {
  WorldGuard g(this);

  NextStep next = FindNextStep();
  if (!next.actor) {
    return false;
  }

  ++step_number_;

  digest_.Eat(next.time).Eat(next.actor_index);

  time_.FastForwardTo(next.time);
  Scope(next.actor)->Step();

  return true;
}

NextStep WorldImpl::FindNextStep() {
  if (actors_.empty()) {
    return NextStep::NoStep();
  }

  auto next_step = NextStep::NoStep();

  for (size_t i = 0; i < actors_.size(); ++i) {
    IActor* actor = actors_[i];

    if (actor->IsRunnable()) {
      TimePoint next_step_time = actor->NextStepTime();

      if (!next_step.actor || next_step_time < next_step.time) {
        next_step.actor = actor;
        next_step.time = next_step_time;
        next_step.actor_index = i;
      }
    }
  }

  return next_step;
}

size_t WorldImpl::Stop() {
  WorldGuard g(this);

  // Adversary
  if (!adversaries_.empty()) {
    Scope(adversaries_.front())->Shutdown();
  }

  LOG_INFO("Adversary stopped");

  // Network
  digest_.Combine(network_.Digest());
  Scope(network_)->Shutdown();

  LOG_INFO("Network stopped");

  // Servers
  for (auto& [_, pool] : pools_) {
    for (auto& server : pool) {
      digest_.Combine(server.ComputeDigest());
      Scope(server)->Shutdown();
    }
    pool.clear();
  }
  pools_.clear();

  LOG_INFO("Servers stopped");

  // Clients
  for (auto& client : clients_) {
    Scope(client)->Shutdown();
  }
  clients_.clear();

  LOG_INFO("Clients stopped");

  actors_.clear();

  history_recorder_.Finalize();

  LOG_INFO("Simulation stopped");

  return Digest();
}

size_t WorldImpl::MakeSteps(size_t steps) {
  size_t steps_made = 0;
  for (size_t i = 0; i < steps; ++i) {
    if (!Step()) {
      break;
    }
    ++steps_made;
  }
  return steps_made;
}

void WorldImpl::RunFor(Jiffies time_budget) {
  while (TimeElapsed() < time_budget) {
    if (!Step()) {
      break;
    }
  }
}

void WorldImpl::RestartServer(const std::string& hostname) {
  WorldGuard g(this);

  FindServer(hostname)->FastReboot();
}

}  // namespace whirl::matrix

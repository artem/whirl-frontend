#pragma once

#include <whirl/matrix/clock.hpp>
#include <whirl/matrix/server.hpp>
#include <whirl/matrix/cluster.hpp>
#include <whirl/matrix/network.hpp>
#include <whirl/matrix/actor.hpp>
#include <whirl/matrix/actor_ctx.hpp>
#include <whirl/matrix/adversary.hpp>
#include <whirl/matrix/random_source.hpp>

#include <whirl/matrix/log.hpp>

#include <deque>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class World {
  struct ScopeGuard {
    ScopeGuard(World* world);
    ~ScopeGuard();
  };

 public:
  World() : network_(clock_) {
  }

  void AddServer(INodeFactoryPtr node_factory) {
    ScopeGuard g(this);

    NodeId id = servers_.size();
    servers_.emplace_back(clock_, network_, NodeConfig{id}, node_factory);
    AddActor(&servers_.back());
  }

  void SetAdversary(AdvStrategy strategy) {
    ScopeGuard g(this);

    WHEELS_VERIFY(!adversary_.has_value(), "Adversary already set");
    adversary_.emplace(std::move(strategy));
    AddActor(&adversary_.value());
  }

  void Start() {
    ScopeGuard g(this);

    // Start network:
    AddActor(&network_);

    std::vector<ServerAddress> peers;
    for (auto& server : servers_) {
      peers.push_back(server.NetAddress());
    }

    // Start servers
    for (auto& server : servers_) {
      server.SetCluster(peers);
      server.Start();
    }

    // Start adversary
    if (adversary_.has_value()) {
      adversary_->Start(WorldView{servers_, network_});
    }

    WHIRL_LLOG("World started");
  }

  bool MakeStep() {
    ScopeGuard g(this);

    IActor* actor = FindNextActor();
    if (!actor) {
      return false;
    }

    ++step_count_;

    clock_.MoveForwardTo(actor->NextStepTime());

    /*
    WHIRL_LLOG("step = " << step_count_ << ", time = " << clock_.Now()
                         << ", actor = " << actor->Name());
    */

    Scope(actor)->MakeStep();

    return true;
  }

  void Stop() {
    ScopeGuard g(this);

    // Adversary
    if (adversary_.has_value()) {
      adversary_->Shutdown();
    }

    WHIRL_LLOG("Adversary stopped");

    // Network
    network_.Shutdown();

    WHIRL_LLOG("Network stopped");

    // Servers
    for (auto& server : servers_) {
      server.Shutdown();
    }
    servers_.clear();

    WHIRL_LLOG("Servers stopped");

    actors_.clear();
  }

  size_t NumServers() {
    return servers_.size();
  }

  // Methods used by running actors

  static World* Access();

  size_t CurrentStep() const {
    return step_count_;
  }

  TimePoint Now() const {
    return clock_.Now();
  }

  RandomSource::ResultType RandomNumber() {
    return random_source_.Next();
  }

  const WorldClock& Clock() const {
    return clock_;
  }

  IActor* CurrentActor() const {
    return current_.Get();
  }

 private:
  ActorContext::ScopeGuard Scope(IActor* actor) {
    return current_.Scope(actor);
  }

  void AddActor(IActor* actor) {
    actors_.push_back(actor);
  }

  IActor* FindNextActor() {
    if (actors_.empty()) {
      return nullptr;
    }

    IActor* next = nullptr;
    for (IActor* actor : actors_) {
      if (actor->IsRunnable()) {
        if (!next || next->NextStepTime() > actor->NextStepTime()) {
          next = actor;
        }
      }
    }

    return next;
  }

 private:
  WorldClock clock_;
  RandomSource random_source_;

  Servers servers_;
  Network network_;
  std::optional<AdversaryProcess> adversary_;

  std::vector<IActor*> actors_;
  ActorContext current_;

  size_t step_count_{0};

  Logger logger_{"World"};
};

}  // namespace whirl

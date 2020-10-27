#pragma once

#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/server/server.hpp>
#include <whirl/matrix/network/network.hpp>
#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/actor_ctx.hpp>
#include <whirl/matrix/adversary/process.hpp>
#include <whirl/matrix/world/random_source.hpp>
#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/history/recorder.hpp>

#include <whirl/matrix/log/logger.hpp>
#include <whirl/matrix/log/log.hpp>

#include <wheels/support/id.hpp>

#include <deque>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class WorldImpl {
  struct WorldGuard {
    WorldGuard(WorldImpl* world);
    ~WorldGuard();
  };

  using Servers = std::deque<Server>;

 public:
  WorldImpl(size_t seed)
      : seed_(seed), random_source_(seed), behaviour_(DefaultBehaviour()) {
  }

  void AddServer(INodeFactoryPtr node) {
    WorldGuard g(this);
    AddServerImpl(cluster_, node, "Server");
  }

  void AddClient(INodeFactoryPtr node) {
    WorldGuard g(this);
    AddServerImpl(clients_, node, "Client");
  }

  void SetAdversary(adversary::Strategy strategy) {
    WorldGuard g(this);

    WHEELS_VERIFY(!adversary_.has_value(), "Adversary already set");
    adversary_.emplace(std::move(strategy));
    AddActor(&adversary_.value());
  }

  void SetBehaviour(IWorldBehaviourPtr behaviour) {
    behaviour_ = std::move(behaviour);
  }

  void Start() {
    WorldGuard g(this);

    SetStartTime();

    // Start network:
    AddActor(&network_);
    Scope(network_)->Start();

    WHIRL_FMT_LOG("Cluster: {}, clients: {}", cluster_.size(), clients_.size());

    WHIRL_FMT_LOG("Starting cluster...");

    // Start servers
    for (auto& server : cluster_) {
      Scope(server)->Start();
    }

    WHIRL_FMT_LOG("Starting clients...");

    // Start clients
    for (auto& client : clients_) {
      Scope(client)->Start();
    }

    // Start adversary
    if (adversary_.has_value()) {
      WHIRL_FMT_LOG("Starting adversary...");
      Scope(*adversary_)->Start();
    }

    WHIRL_LOG("World started");
  }

  bool Step() {
    WorldGuard g(this);

    IActor* actor = FindNextActor();
    if (!actor) {
      return false;
    }

    ++step_count_;

    clock_.MoveForwardTo(actor->NextStepTime());
    Scope(actor)->Step();

    return true;
  }

  void MakeSteps(size_t steps) {
    for (size_t i = 0; i < steps; ++i) {
      if (!Step()) {
        break;
      }
    }
  }

  void RunFor(Duration d) {
    while (Now() < d) {
      if (!Step()) {
        break;
      }
    }
  }

  size_t Stop() {
    WorldGuard g(this);

    // Adversary
    if (adversary_.has_value()) {
      Scope(*adversary_)->Shutdown();
    }

    WHIRL_LOG("Adversary stopped");

    // Network
    Scope(network_)->Shutdown();

    WHIRL_LOG("Network stopped");

    // Servers
    for (auto& server : cluster_) {
      Scope(server)->Shutdown();
    }
    cluster_.clear();

    WHIRL_LOG("Servers stopped");

    // Clients
    for (auto& client : clients_) {
      Scope(client)->Shutdown();
    }
    clients_.clear();

    WHIRL_LOG("Clients stopped");

    actors_.clear();

    history_recorder_.Finalize();

    WHIRL_LOG("Simulation stopped");

    return 0;  // TODO
  }

  size_t NumServers() const {
    return cluster_.size();
  }

  size_t NumClients() const {
    return clients_.size();
  }

  // Methods used by running actors

  static WorldImpl* Access();

  size_t Seed() const {
    return seed_;
  }

  Server& GetServer(size_t index) {
    return cluster_.at(index);
  }

  Network& GetNetwork() {
    return network_;
  }

  size_t CurrentStep() const {
    return step_count_;
  }

  Log& GetLog() {
    return log_;
  }

  histories::Recorder& HistoryRecorder() {
    return history_recorder_;
  }

  // Context: Server
  std::vector<std::string> ClusterAddresses() {
    std::vector<ServerAddress> addrs;
    addrs.reserve(cluster_.size());

    for (auto& server : cluster_) {
      addrs.push_back(server.NetAddress());
    }
    return addrs;
  }

  const histories::History& History() const {
    return history_recorder_.GetHistory();
  }

  TimePoint Now() const {
    return clock_.Now();
  }

  RandomSource::ResultType RandomNumber() {
    return random_source_.Next();
  }

  const IWorldBehaviourPtr& Behaviour() const {
    return behaviour_;
  }

  IActor* CurrentActor() const {
    return active_.Get();
  }

  // Statistics

  size_t PacketsSent() const {
    return network_.PacketsSent();
  }

 private:
  void AddServerImpl(Servers& servers, INodeFactoryPtr node, std::string type) {
    size_t id = ids_.NextId();
    std::string name = type + "-" + std::to_string(servers.size() + 1);

    servers.emplace_back(network_, ServerConfig{id, name}, node);
    AddActor(&servers.back());
  }

  void SetStartTime() {
    clock_.MoveForwardTo(GlobalStartTime());
  }

  ActorContext::ScopeGuard Scope(IActor& actor) {
    return Scope(&actor);
  }

  ActorContext::ScopeGuard Scope(IActor* actor) {
    return active_.Scope(actor);
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
  size_t seed_;

  WorldClock clock_;
  RandomSource random_source_;

  wheels::support::IdGenerator ids_;

  IWorldBehaviourPtr behaviour_;

  // Actors

  Servers cluster_;
  Servers clients_;

  Network network_;

  std::optional<adversary::Process> adversary_;

  // Theatre

  std::vector<IActor*> actors_;
  ActorContext active_;
  size_t step_count_{0};

  Logger logger_{"World"};

  Log log_;
  histories::Recorder history_recorder_;
};

}  // namespace whirl

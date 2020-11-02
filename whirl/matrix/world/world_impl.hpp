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

#include <whirl/helpers/untyped_dict.hpp>

#include <wheels/support/id.hpp>

#include <await/fibers/core/guts.hpp>

#include <deque>
#include <vector>

namespace whirl {

//////////////////////////////////////////////////////////////////////

struct NextStep {
  IActor* actor;
  TimePoint time;

  static NextStep NoStep() {
    return {nullptr, 0};
  }
};

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

  bool HasAdversary() const {
    return adversary_.has_value();
  }

  void SetBehaviour(IWorldBehaviourPtr behaviour) {
    behaviour_ = std::move(behaviour);
  }

  void Start() {
    WorldGuard g(this);

    SetStartTime();
    start_time_ = clock_.Now();

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

    NextStep next = FindNextStep();
    if (!next.actor) {
      return false;
    }

    ++step_count_;

    clock_.MoveForwardTo(next.time);
    Scope(next.actor)->Step();

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

    CheckNoFibersLeft();

    history_recorder_.Finalize();

    WHIRL_LOG("Simulation stopped");

    return ComputeDigest();
  }

  size_t ClusterSize() const {
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

  net::Network& GetNetwork() {
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
  std::vector<std::string> GetCluster() {
    std::vector<std::string> cluster;
    cluster.reserve(cluster_.size());

    for (auto& server : cluster_) {
      cluster.push_back(server.HostName());
    }
    return cluster;
  }

  const histories::History& History() const {
    return history_recorder_.GetHistory();
  }

  TimePoint Now() const {
    return clock_.Now();
  }

  Duration TimeElapsed() const {
    return clock_.Now() - start_time_;
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

  void SetGlobal(const std::string& name, std::any value) {
    globals_.Set(name, value);
  }

  std::any GetGlobal(const std::string& name) const {
    return globals_.Get(name);
  }

 private:
  void AddServerImpl(Servers& servers, INodeFactoryPtr node, std::string type) {
    size_t id = ids_.NextId();
    std::string name = type + "-" + std::to_string(servers.size() + 1);

    servers.emplace_back(network_, ServerConfig{id, name}, node);

    network_.AddServer(&servers.back());
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

  NextStep FindNextStep() {
    if (actors_.empty()) {
      return NextStep::NoStep();
    }

    auto next_step = NextStep::NoStep();

    for (IActor* actor : actors_) {
      if (actor->IsRunnable()) {
        TimePoint next_step_time = actor->NextStepTime();

        if (!next_step.actor || next_step_time < next_step.time) {
          next_step.actor = actor;
          next_step.time = next_step_time;
        }
      }
    }

    return next_step;
  }

  size_t ComputeDigest() const {
    return clock_.Now() * 31007 + step_count_ * 40013;
  }

  void CheckNoFibersLeft() {
    WHEELS_VERIFY(await::fibers::AliveFibers().IsEmpty(), "Alive fibers!");
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

  net::Network network_;

  std::optional<adversary::Process> adversary_;

  // Theatre

  std::vector<IActor*> actors_;
  ActorContext active_;
  size_t step_count_{0};

  Logger logger_{"World"};

  TimePoint start_time_;

  Log log_;
  histories::Recorder history_recorder_;

  UntypedDict globals_;
};

}  // namespace whirl

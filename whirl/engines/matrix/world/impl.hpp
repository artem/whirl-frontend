#pragma once

#include <whirl/engines/matrix/world/time.hpp>
#include <whirl/engines/matrix/server/server.hpp>
#include <whirl/engines/matrix/network/network.hpp>
#include <whirl/engines/matrix/world/actor.hpp>
#include <whirl/engines/matrix/world/actor_ctx.hpp>
#include <whirl/engines/matrix/adversary/process.hpp>
#include <whirl/engines/matrix/world/random_source.hpp>
#include <whirl/engines/matrix/world/time_model.hpp>
#include <whirl/engines/matrix/history/recorder.hpp>
#include <whirl/engines/matrix/log/backend.hpp>

#include <whirl/engines/matrix/helpers/digest.hpp>
#include <whirl/engines/matrix/helpers/untyped_dict.hpp>

#include <whirl/services/guid.hpp>

#include <whirl/logger/log.hpp>

#include <wheels/support/id.hpp>

#include <await/fibers/core/guts.hpp>

#include <deque>
#include <vector>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct NextStep {
  IActor* actor;
  TimePoint time;
  size_t actor_index;

  static NextStep NoStep() {
    return {nullptr, 0, 0};
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
      : seed_(seed), random_source_(seed), time_model_(DefaultTimeModel()) {
  }

  std::string AddServer(node::INodeFactoryPtr node) {
    WorldGuard g(this);
    return AddServerImpl(cluster_, node, "Server");
  }

  std::string AddClient(node::INodeFactoryPtr node) {
    WorldGuard g(this);
    return AddServerImpl(clients_, node, "Client");
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

  void SetTimeModel(ITimeModelPtr time_model) {
    time_model_ = std::move(time_model);
  }

  void Start();

  // Returns false if simulation is in deadlock state
  bool Step();

  // Stop simulation and compute digest
  size_t Stop();

  // Returns number of steps actually made
  size_t MakeSteps(size_t steps);

  void RunFor(Duration time_budget);

  void RestartServer(const std::string& hostname);

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

  size_t Digest() const {
    return digest_.GetValue();
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

  LogBackend& GetLog() {
    return log_;
  }

  HistoryRecorder& GetHistoryRecorder() {
    return history_recorder_;
  }

  // Context: Server
  std::vector<std::string> GetPool(const std::string& name) {
    if (name != "cluster") {
      WHEELS_PANIC("Custom pools are not supported yet!");
    }

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

  std::vector<std::string> GetStdout(const std::string& hostname) {
    const Server* server = FindServer(hostname);
    return server->GetStdout();
  }

  TimePoint Now() const {
    return time_.Now();
  }

  Duration TimeElapsed() const {
    return time_.Now() - start_time_;
  }

  size_t StepRandomNumber() const {
    return step_random_number_;
  }

  uint64_t RandomNumber() {
    return random_source_.Next();
  }

  const ITimeModelPtr& TimeModel() const {
    return time_model_;
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

  Guid GenerateGuid() {
    return fmt::format("guid-{}", guids_.NextId());
  }

 private:
  static ITimeModelPtr DefaultTimeModel();

  // Returns host name
  std::string AddServerImpl(Servers& servers, node::INodeFactoryPtr node, std::string type) {
    size_t id = server_ids_.NextId();
    std::string name = type + "-" + std::to_string(servers.size() + 1);

    servers.emplace_back(network_, ServerConfig{id, name}, node);

    network_.AddServer(&servers.back());
    AddActor(&servers.back());

    return name;
  }

  Server* FindServer(const std::string& hostname) {
    for (Server& server : cluster_) {
      if (server.HostName() == hostname) {
        return &server;
      }
    }
    return nullptr;
  }

  void SetStartTime() {
    time_.FastForwardTo(GetTimeModel()->GlobalStartTime());
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

  NextStep FindNextStep();

  void CheckNoFibersLeft() {
    WHEELS_VERIFY(await::fibers::AliveFibers().IsEmpty(), "Alive fibers!");
  }

 private:
  const size_t seed_;

  Time time_;
  RandomSource random_source_;
  wheels::IdGenerator guids_;

  ITimeModelPtr time_model_;

  // Actors

  Servers cluster_;
  Servers clients_;

  wheels::IdGenerator server_ids_;

  net::Network network_;

  std::optional<adversary::Process> adversary_;

  // Event loop

  std::vector<IActor*> actors_;
  ActorContext active_;

  size_t step_count_{0};

  Logger logger_{"World"};

  TimePoint start_time_;

  size_t step_random_number_{0};

  DigestCalculator digest_;
  LogBackend log_;
  HistoryRecorder history_recorder_;

  UntypedDict globals_;
};

}  // namespace whirl::matrix

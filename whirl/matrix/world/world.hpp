#pragma once

#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/adversary/strategy.hpp>
#include <whirl/matrix/history/history.hpp>
#include <whirl/node/node.hpp>

#include <memory>

namespace whirl {

class WorldImpl;

// Facade

class World {
  static const size_t kDefaultSeed = 42;

 public:
  World(size_t seed = kDefaultSeed);
  ~World();

  void AddServer(INodeFactoryPtr node);
  void AddServers(size_t count, INodeFactoryPtr node);

  void AddClient(INodeFactoryPtr node);
  void AddClients(size_t count, INodeFactoryPtr node);

  void SetBehaviour(IWorldBehaviourPtr behaviour);

  void SetAdversary(adversary::Strategy strategy);

  void Start();

  bool Step();
  void MakeSteps(size_t count);

  void Stop();

  const histories::History& History() const;

 private:
  std::unique_ptr<WorldImpl> impl_;
};

}  // namespace whirl

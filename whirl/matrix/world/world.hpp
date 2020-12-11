#pragma once

#include <whirl/matrix/world/behaviour.hpp>
#include <whirl/matrix/adversary/strategy.hpp>
#include <whirl/matrix/history/history.hpp>
#include <whirl/node/node.hpp>

#include <memory>
#include <ostream>
#include <any>

namespace whirl {

class WorldImpl;

// Facade

class World {
  static const size_t kDefaultSeed = 42;

 public:
  World(size_t seed = kDefaultSeed);
  ~World();

  size_t Seed() const;

  void AddServer(INodeFactoryPtr node);
  void AddServers(size_t count, INodeFactoryPtr node);

  void AddClient(INodeFactoryPtr node);
  void AddClients(size_t count, INodeFactoryPtr node);

  void SetBehaviour(IWorldBehaviourPtr behaviour);

  void SetAdversary(adversary::Strategy strategy);

  // Globals

  template <typename T>
  void SetGlobal(const std::string& key, T value) {
    SetGlobalImpl(key, value);
  }

  template <typename T>
  T GetGlobal(const std::string& key) const {
    auto value = GetGlobalImpl(key);
    return std::any_cast<T>(value);
  }

  // Global counters

  void InitCounter(const std::string& name, size_t value = 0) {
    SetGlobal(name, value);
  }

  size_t GetCounter(const std::string& name) const {
    return GetGlobal<size_t>(name);
  }


  void WriteLogTo(std::ostream& out);

  void Start();

  bool Step();
  void MakeSteps(size_t count);

  // Returns simulation digest
  size_t Stop();

  size_t Digest() const;

  size_t NumCompletedCalls() const;

  size_t StepCount() const;
  Duration TimeElapsed() const;

  const histories::History& History() const;

 private:
  void SetGlobalImpl(const std::string& key, std::any value);
  std::any GetGlobalImpl(const std::string& key) const;

 private:
  std::unique_ptr<WorldImpl> impl_;
};

}  // namespace whirl

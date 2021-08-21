#pragma once

#include <whirl/engines/matrix/world/time_model.hpp>
#include <whirl/engines/matrix/log/event.hpp>
#include <whirl/history/history.hpp>
#include <whirl/program/program.hpp>

#include <memory>
#include <ostream>
#include <any>

namespace whirl::matrix {

// Facade

//////////////////////////////////////////////////////////////////////

class World;

//////////////////////////////////////////////////////////////////////

class PoolBuilder {
 public:
  PoolBuilder(World* world, std::string pool_name, node::Program program)
      : world_(world),
        pool_name_(pool_name),
        program_(program),
        name_template_(MakeNameTemplate(pool_name)) {
  }

  PoolBuilder& Size(size_t value) {
    size_ = value;
    return *this;
  }

  PoolBuilder& NameTemplate(std::string value) {
    name_template_ = value;
    return *this;
  }

  // Add pool to the world
  ~PoolBuilder();

 private:
  static std::string MakeNameTemplate(std::string pool_name) {
    return std::string("Server-") + pool_name;
  }

 private:
  World* world_;

  std::string pool_name_;
  node::Program program_;
  size_t size_ = 1;
  std::string name_template_;
};

//////////////////////////////////////////////////////////////////////

class WorldImpl;

class World {
  static const size_t kDefaultSeed = 42;

  friend class PoolBuilder;

 public:
  World(size_t seed = kDefaultSeed);
  ~World();

  size_t Seed() const;

  void AddServer(std::string hostname, node::Program program);

  PoolBuilder MakePool(std::string pool_name, node::Program program) {
    return PoolBuilder{this, pool_name, program};
  }

  void AddClient(node::Program program);
  void AddClients(node::Program program, size_t count);

  void SetTimeModel(ITimeModelPtr time_model);

  void AddAdversary(node::Program program);

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

  void Start();

  bool Step();
  void MakeSteps(size_t count);

  // For tests
  void RestartServer(const std::string& hostname);

  // Returns simulation digest
  size_t Stop();

  size_t Digest() const;

  size_t StepCount() const;
  Duration TimeElapsed() const;

  const EventLog& EventLog() const;
  const histories::History& History() const;

  std::vector<std::string> GetStdout(const std::string& hostname) const;

 private:
  void AddPool(std::string pool_name,
               node::Program program,
               size_t size,
               std::string server_name_template);

  void SetGlobalImpl(const std::string& key, std::any value);
  std::any GetGlobalImpl(const std::string& key) const;

 private:
  std::unique_ptr<WorldImpl> impl_;
};

}  // namespace whirl::matrix

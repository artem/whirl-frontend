#pragma once

#include <whirl/matrix/world/faults.hpp>
#include <whirl/matrix/adversary/strategy.hpp>
#include <whirl/matrix/log/logger.hpp>

#include <vector>

namespace whirl::adversary {


struct AdversaryBase {
 public:
  AdversaryBase(ThreadsRuntime runtime) : runtime_(runtime) {
  }

  void Start();

 protected:
  // Override this

  virtual void Initialize() {
    // Your code goes here
  }

  virtual void AbuseWholeCluster(std::vector<IFaultyServer*>) {
    // Your code goes here
  }

  virtual void AbuseNetwork(IFaultyNetwork&) {
    // Your code goes here
  }

  virtual void AbuseServer(IFaultyServer&) {
    // Your code goes here
  }

 protected:
  ThreadsRuntime& Threads() {
    return runtime_;
  }

  void RandomPause(size_t lo, size_t hi);

 private:
  ThreadsRuntime runtime_;

 protected:
  Logger logger_{"Adversary"};
};

// This is ugly, we need good representations for program/process
template <typename TAdversary>
Strategy MakeStrategy() {
  auto strategy = [](ThreadsRuntime& runtime) {
    TAdversary instance{runtime};
    instance.Start();
    runtime.SleepFor(100500);
  };

  return strategy;
}

}  // namespace whirl::adversary
#pragma once

#include <whirl/node/services.hpp>

namespace whirl {

// Accessors for node services

class ThisNode {
 public:
  ThisNode(NodeServices services) : services_(std::move(services)) {
  }

 protected:
  const NodeServices& ThisNodeServices() {
    return services_;
  }

  // Randomness

  size_t RandomNumber() {
    return services_.random->RandomNumber();
  }

  // [0, bound)
  size_t RandomNumber(size_t bound) {
    return RandomNumber() % bound;
  }

  // [lo, hi)
  size_t RandomNumber(size_t lo, size_t hi) {
    return lo + RandomNumber(hi - lo);
  }

  // Clocks

  TimePoint WallTimeNow() {
    return services_.time_service->WallTimeNow();
  }

  TimePoint MonotonicNow() {
    return services_.time_service->MonotonicNow();
  }

  // TrueTime

  TTNow TrueTimeNow() {
    return services_.true_time->Now();
  }

  const ITrueTimeServicePtr& TrueTime() {
    return services_.true_time;
  }

  // Local storage backend

  const ILocalStorageBackendPtr& StorageBackend() {
    return services_.storage_backend;
  }

  // Logger
  // Usage: NODE_LOG(format, values...)

  INodeLoggerPtr NodeLogger() {
    return services_.logger;
  }

 private:
  NodeServices services_;
};

}  // namespace whirl

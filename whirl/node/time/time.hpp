#pragma once

#include <whirl/node/time/time_point.hpp>

#include <whirl/time.hpp>

#include <await/futures/core/future.hpp>

#include <memory>

namespace whirl::node::time {

struct ITimeService {
  virtual ~ITimeService() = default;

  // Wall clock
  virtual WallTime WallTimeNow() = 0;

  // Monotonic (steady) clock
  virtual MonotonicTime MonotonicNow() = 0;

  // Timeouts and delays
  virtual await::futures::Future<void> After(Jiffies d) = 0;
};

}  // namespace whirl::node::time

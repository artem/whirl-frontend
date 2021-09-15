#pragma once

#include <whirl/time.hpp>

#include <await/futures/core/future.hpp>

#include <memory>

namespace whirl::node::time {

struct ITimeService {
  virtual ~ITimeService() = default;

  // Wall clock
  virtual TimePoint WallTimeNow() = 0;

  // Monotonic (steady) clock
  virtual TimePoint MonotonicNow() = 0;

  // Timeouts and delays
  virtual await::futures::Future<void> After(Jiffies d) = 0;
};

}  // namespace whirl::node::time

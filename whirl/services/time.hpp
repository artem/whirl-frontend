#pragma once

#include <wheels/support/unit.hpp>

#include <await/futures/future.hpp>

#include <whirl/matrix/world/time.hpp>

namespace whirl {

using await::futures::Future;
using wheels::Unit;

struct ITimeService {
  virtual ~ITimeService() = default;

  // Wall-time (system) clock
  virtual TimePoint WallTimeNow() = 0;

  // Monotonic (steady) clock
  virtual TimePoint MonotonicNow() = 0;

  // Timeouts and delays
  virtual Future<void> After(Duration d) = 0;
};

using ITimeServicePtr = std::shared_ptr<ITimeService>;

}  // namespace whirl

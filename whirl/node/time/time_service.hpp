#pragma once

#include <await/time/timer_service.hpp>

#include <whirl/node/time/jiffies.hpp>
#include <whirl/node/time/wall_time.hpp>
#include <whirl/node/time/monotonic_time.hpp>

namespace whirl::node::time {

struct ITimeService : public await::time::ITimerService {
  virtual ~ITimeService() = default;

  // Wall clock
  virtual WallTime WallTimeNow() = 0;

  // Monotonic (steady) clock
  virtual MonotonicTime MonotonicNow() = 0;

  // Timeouts and delays
  // Inherited from await::time::ITimerService
  // virtual await::futures::Future<void> After(await::time::Jiffies d) = 0;
};

}  // namespace whirl::node::time

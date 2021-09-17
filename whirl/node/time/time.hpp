#pragma once

#include <await/time/timer_service.hpp>

#include <whirl/node/time/time_point.hpp>

#include <whirl/time.hpp>

#include <memory>

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

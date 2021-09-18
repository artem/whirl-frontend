#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/network/packet.hpp>
#include <whirl/engines/matrix/network/server.hpp>

#include <commute/rpc/retries.hpp>

#include <memory>

namespace whirl::matrix {

struct ITimeModel {
  virtual ~ITimeModel() = default;

  virtual TimePoint GlobalStartTime() = 0;

  // Clocks

  virtual int InitClockDrift() = 0;
  virtual int ClockDriftBound() = 0;

  // Monotonic clock

  virtual TimePoint ResetMonotonicClock() = 0;

  // Wall clock

  virtual TimePoint InitWallClockOffset() = 0;

  // TrueTime

  virtual Jiffies TrueTimeUncertainty() = 0;

  // Disk

  virtual Jiffies DiskWrite(size_t bytes) = 0;
  virtual Jiffies DiskRead(size_t bytes) = 0;

  // Network

  // DPI =)
  virtual Jiffies FlightTime(const net::IServer* start, const net::IServer* end,
                             const net::Packet& packet) = 0;

  virtual commute::rpc::BackoffParams BackoffParams() = 0;

  // Threads

  virtual Jiffies ThreadPause() = 0;
};

using ITimeModelPtr = std::shared_ptr<ITimeModel>;

}  // namespace whirl::matrix

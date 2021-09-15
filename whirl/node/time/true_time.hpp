#pragma once

#include <whirl/time.hpp>

#include <whirl/node/time/time_point.hpp>

#include <memory>

namespace whirl::node::time {

// TrueTime from Spanner
// Spanner: Google's Globally-Distributed Database"
// https://research.google/pubs/pub39966/

// [earliest, latest]
struct TTInterval {
  WallTime earliest;
  WallTime latest;
};

struct ITrueTimeService {
  virtual ~ITrueTimeService() = default;

  // Returns a TTInterval that is guaranteed to contain
  // the absolute time during which TT.now() was invoked
  virtual TTInterval Now() const = 0;

  // True if `t` has definitely passed
  bool After(WallTime t) const {
    return Now().earliest > t;
  }

  // True if `t` has definitely not arrived
  bool Before(WallTime t) const {
    return Now().latest < t;
  }
};

}  // namespace whirl::node::time

#pragma once

#include <whirl/time.hpp>

#include <memory>

namespace whirl {

// TrueTime from Spanner
// Spanner: Google's Globally-Distributed Database"
// https://research.google/pubs/pub39966/

//////////////////////////////////////////////////////////////////////

struct TTNow {
  TimePoint earliest;
  TimePoint latest;
};

//////////////////////////////////////////////////////////////////////

struct ITrueTimeService {
  virtual ~ITrueTimeService() = default;

  virtual TTNow Now() const = 0;

  // True if t has definitely passed
  bool After(TimePoint t) const {
    return t < Now().earliest;
  }

  // True if t has definitely not arrived
  bool Before(TimePoint t) const {
    return t > Now().latest;
  }
};

using ITrueTimeServicePtr = std::shared_ptr<ITrueTimeService>;

}  // namespace whirl

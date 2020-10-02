#pragma once

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

  virtual TTNow Now() = 0;

  bool Before(TimePoint tp) {
    return tp < Now().earliest;
  }

  bool After(TimePoint tp) {
    return tp > Now().latest;
  }
};

using ITrueTimeServicePtr = std::shared_ptr<ITrueTimeService>;

}  // namespace whirl

#pragma once

#include <whirl/history/history.hpp>

namespace whirl::histories {

// https://jepsen.io/consistency/models/linearizable

inline bool PrecedesInRealTime(const Call& lhs, const Call& rhs) {
  return lhs.IsCompleted() && lhs.end_time.value() < rhs.start_time;
}

}  // namespace whirl::histories

#pragma once

#include <whirl/matrix/history/history.hpp>

namespace whirl::histories {

// See Linearizability consistency model
inline bool PrecedesInRealTime(const Call& lhs, const Call& rhs) {
  return lhs.IsCompleted() && lhs.end_time.value() < rhs.start_time;
}

}  // namespace whirl::histories

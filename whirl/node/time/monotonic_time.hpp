#pragma once

#include <whirl/node/time/jiffies.hpp>

namespace whirl::node::time {

///////////////////////////////////////////////////////////////////////////

class MonotonicTime {
 public:
  MonotonicTime(Jiffies jfs) : jfs_(jfs) {
  }

  // Time elapsed since monotonic clock reset
  Jiffies ToJiffies() const {
    return jfs_;
  }

 private:
  Jiffies jfs_;
};

///////////////////////////////////////////////////////////////////////////

inline Jiffies operator-(MonotonicTime end, MonotonicTime start) {
  return start.ToJiffies() - end.ToJiffies();
}

}  // namespace whirl::node::time

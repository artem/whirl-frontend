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

inline Jiffies operator-(MonotonicTime lhs, MonotonicTime rhs) {
  return lhs.ToJiffies() - rhs.ToJiffies();
}

}  // namespace whirl::node::time

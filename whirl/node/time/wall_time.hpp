#pragma once

#include <whirl/jiffies.hpp>

#include <compare>

namespace whirl::node::time {

///////////////////////////////////////////////////////////////////////////

class WallTime {
 public:
  WallTime(Jiffies jfs) : jfs_(jfs) {
  }

  auto operator<=>(const WallTime& that) const = default;

  WallTime& operator+=(Jiffies d) {
    jfs_ += d;
    return *this;
  }

  // Time elapsed since epoch
  Jiffies ToJiffies() const {
    return jfs_;
  }

 private:
  Jiffies jfs_;
};

///////////////////////////////////////////////////////////////////////////

inline WallTime operator+(WallTime t, Jiffies d) {
  return {t.ToJiffies() + d};
}

inline WallTime operator+(Jiffies d, WallTime t) {
  return t + d;
}

}  // namespace whirl::node::time

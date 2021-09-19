#pragma once

#include <whirl/jiffies.hpp>

#include <compare>
#include <ostream>

namespace whirl::node::time {

///////////////////////////////////////////////////////////////////////////

class WallTime {
 public:
  WallTime(Jiffies jfs) : jfs_(jfs) {
  }

  // Comparison
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

inline WallTime operator+(WallTime t, Jiffies d) {
  return {t.ToJiffies() + d};
}

///////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& out, const WallTime& t) {
  out << 'T' << t.ToJiffies().Count();
  return out;
}

}  // namespace whirl::node::time

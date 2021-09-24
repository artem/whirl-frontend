#pragma once

#include <whirl/node/time/jiffies.hpp>

#include <ostream>

namespace whirl::node::time {

///////////////////////////////////////////////////////////////////////////

class WallTime {
 public:
  WallTime(Jiffies jfs) : jfs_(jfs) {
  }

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

inline Jiffies operator-(WallTime lhs, WallTime rhs) {
  return lhs.ToJiffies() - rhs.ToJiffies();
}

inline bool operator==(WallTime lhs, WallTime rhs) {
  return lhs.ToJiffies() == rhs.ToJiffies();
}

inline bool operator!=(WallTime lhs, WallTime rhs) {
  return !(lhs == rhs);
}

inline bool operator<(WallTime lhs, WallTime rhs) {
  return lhs.ToJiffies() < rhs.ToJiffies();
}

inline bool operator>(WallTime lhs, WallTime rhs) {
  return lhs.ToJiffies() > rhs.ToJiffies();
}

///////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& out, const WallTime& t) {
  out << 'T' << t.ToJiffies().Count();
  return out;
}

}  // namespace whirl::node::time

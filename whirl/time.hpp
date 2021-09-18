#pragma once

#include <await/time/jiffies.hpp>

#include <fmt/ostream.h>

#include <cstdlib>
#include <compare>
#include <iostream>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time measured in abstract `jiffies` - Paxon unit of time
// https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf, page 13, footnote
// 11

using TimePoint = size_t;

class Jiffies {
 public:
  Jiffies(uint64_t count) : count_(count) {
  }

  uint64_t Count() const {
    return count_;
  }

  auto operator<=>(const Jiffies& rhs) const = default;

  Jiffies& operator+=(Jiffies rhs) {
    count_ += rhs.count_;
    return *this;
  }

  Jiffies& operator-=(Jiffies rhs) {
    count_ -= rhs.count_;
    return *this;
  }

 private:
  uint64_t count_;
};

inline Jiffies operator+(Jiffies lhs, Jiffies rhs) {
  return {lhs.Count() + rhs.Count()};
}

inline Jiffies operator-(Jiffies lhs, Jiffies rhs) {
  return {lhs.Count() - rhs.Count()};
}

//////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& out, Jiffies jfs) {
  out << jfs.Count() << "jfs";
  return out;
}

//////////////////////////////////////////////////////////////////////

// Literals for jiffies

inline Jiffies operator""_jiffies(unsigned long long int d) {
  return Jiffies{d};
}

// More compact
inline Jiffies operator""_jfs(unsigned long long int d) {
  return Jiffies{d};
}

}  // namespace whirl

//////////////////////////////////////////////////////////////////////

// Integration with await
// await::time::Jiffies == whirl::Jiffies

namespace await::time {

template <>
inline Jiffies ToJiffies(whirl::Jiffies jfs) {
  return jfs.Count();
}

}  // namespace await::time

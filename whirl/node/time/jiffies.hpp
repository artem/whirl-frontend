#pragma once

#include <await/time/jiffies.hpp>

#include <fmt/ostream.h>

#include <cstdlib>
#include <iostream>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time measured in abstract `jiffies` - Paxon unit of time
// https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf, page 13, footnote
// 11

class Jiffies {
 public:
  using ValueType = uint64_t;

 public:
  Jiffies(ValueType count) : count_(count) {
  }

  ValueType Count() const {
    return count_;
  }

  Jiffies& operator+=(Jiffies rhs) {
    count_ += rhs.count_;
    return *this;
  }

  Jiffies& operator-=(Jiffies rhs) {
    count_ -= rhs.count_;
    return *this;
  }

 private:
  ValueType count_;
};

inline Jiffies operator+(Jiffies lhs, Jiffies rhs) {
  return {lhs.Count() + rhs.Count()};
}

inline Jiffies operator-(Jiffies lhs, Jiffies rhs) {
  return {lhs.Count() - rhs.Count()};
}

//////////////////////////////////////////////////////////////////////

// Comparison

inline bool operator==(Jiffies lhs, Jiffies rhs) {
  return lhs.Count() == rhs.Count();
}

inline bool operator!=(Jiffies lhs, Jiffies rhs) {
  return !(lhs == rhs);
}

inline bool operator<(Jiffies lhs, Jiffies rhs) {
  return lhs.Count() < rhs.Count();
}

inline bool operator>(Jiffies lhs, Jiffies rhs) {
  return lhs.Count() > rhs.Count();
}

inline bool operator<=(Jiffies lhs, Jiffies rhs) {
  return lhs.Count() <= rhs.Count();
}

inline bool operator>=(Jiffies lhs, Jiffies rhs) {
  return lhs.Count() >= rhs.Count();
}

//////////////////////////////////////////////////////////////////////

// Support for std streams and fmtlb (+ fmt/ostream.h)

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
// await::time::Jiffies = whirl::Jiffies

namespace await::time {

template <>
inline Jiffies ToJiffies(whirl::Jiffies jfs) {
  return jfs.Count();
}

}  // namespace await::time

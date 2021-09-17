#pragma once

#include <await/time/jiffies.hpp>

#include <cstdlib>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time measured in abstract `jiffies` - Paxon unit of time
// https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf, page 13, footnote
// 11

using TimePoint = size_t;

using Jiffies = uint64_t;

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

// await::time::Jiffies == whirl::Jiffies

namespace await::time {

template <>
inline Jiffies ToJiffies(whirl::Jiffies jfs) {
  return jfs;
}

}  // namespace await::time

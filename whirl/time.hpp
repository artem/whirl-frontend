#pragma once

#include <cstdlib>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time measured in abstract `jiffies` - Paxon unit of time
// https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf, page 13, footnote
// 11

using TimePoint = size_t;

using Duration = size_t;

//////////////////////////////////////////////////////////////////////

namespace time_literals {

// Literals for jiffies

inline Duration operator""_jiffies(unsigned long long int d) {
  return Duration{d};
}

// More compact
inline Duration operator""_jfs(unsigned long long int d) {
  return Duration{d};
}

}  // namespace time_literals

}  // namespace whirl

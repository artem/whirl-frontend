#pragma once

#include <cstdlib>

namespace whirl {

// Time measured in some abstract `jiffies` - Paxon unit of time
// https://lamport.azurewebsites.net/pubs/lamport-paxos.pdf, page 13, footnote 11

using TimePoint = size_t;

using Duration = size_t;

}  // namespace whirl

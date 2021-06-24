#pragma once

#include <cstdlib>

namespace whirl::matrix::net {

// Each endpoint receives new timestamp on creation
// packet.ts < endpoint.ts => packet is outdated,

using Timestamp = size_t;

// Non-decreasing
Timestamp GetNewEndpointTimestamp();

}  // namespace whirl::matrix::net

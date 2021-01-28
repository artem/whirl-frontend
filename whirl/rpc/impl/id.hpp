#pragma once

#include <cstdlib>

namespace whirl::rpc {

using RequestId = size_t;

RequestId GenerateRequestId();

// Workaround for deterministic simulation
void ResetIds();

}  // namespace whirl::rpc

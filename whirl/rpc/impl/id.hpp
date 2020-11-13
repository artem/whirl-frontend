#pragma once

#include <cstdlib>

namespace whirl::rpc {

using RequestId = size_t;

RequestId GenerateRequestId();

void ResetIds();

}  // namespace whirl::rpc

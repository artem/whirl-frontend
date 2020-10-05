#pragma once

#include <cstdlib>

namespace whirl::rpc {

using RPCId = size_t;

RPCId GenerateRequestId();

}  // namespace whirl::rpc

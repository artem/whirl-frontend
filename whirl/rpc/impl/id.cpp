#include <whirl/rpc/impl/id.hpp>

#include <wheels/support/id.hpp>

namespace whirl::rpc {

static wheels::support::IdGenerator ids_;

RPCId GenerateRequestId() {
  return ids_.NextId();
}

void ResetIds() {
  ids_.Reset();
}

}  // namespace whirl::rpc

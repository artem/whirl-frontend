#include <whirl/rpc/impl/id.hpp>

#include <wheels/support/id.hpp>

namespace whirl::rpc {

RPCId GenerateRequestId() {
  static wheels::support::IdGenerator ids_;

  return ids_.NextId();
}

}  // namespace whirl::rpc

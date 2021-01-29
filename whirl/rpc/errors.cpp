#include <whirl/rpc/errors.hpp>

namespace whirl::rpc {

bool IsRetriableError(RPCErrorCode e) {
  switch (e) {
    case RPCErrorCode::TransportError: return true;
    default: return false;
  }
}

}  // namespace whirl::rpc

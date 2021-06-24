#include <whirl/engines/matrix/network/timestamp.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

namespace whirl::matrix::net {

Timestamp GetNewEndpointTimestamp() {
  return WorldStepNumber();
}

}  // namespace whirl::matrix::net
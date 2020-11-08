#include <whirl/matrix/network/timestamp.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl::net {

Timestamp GetNewEndpointTimestamp() {
  return WorldStepNumber();
}

}  // namespace whirl::net
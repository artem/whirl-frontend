#include <whirl/matrix/network/timestamp.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl::net {

Timestamp GetEndpointTimestamp() {
  return WorldStepNumber();
}

}  // namespace whirl::net
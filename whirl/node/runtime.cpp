#include <whirl/node/runtime.hpp>

// It is OK!
#include <whirl/engines/matrix/server/server.hpp>

namespace whirl {

const NodeRuntime& GetRuntime() {
  return matrix::ThisServer().GetNodeRuntime();
}

}  // namespace whirl

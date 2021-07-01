#include <whirl/node/runtime.hpp>

#include <whirl/engines/reflect.hpp>

// It is OK!
#include <whirl/engines/matrix/server/server.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

INodeRuntime& GetRuntime() {
  if (IsMatrix()) {
    return matrix::ThisServer().GetNodeRuntime();
  }
  WHEELS_PANIC("GetRuntime is not supported for current engine");
}

}  // namespace whirl

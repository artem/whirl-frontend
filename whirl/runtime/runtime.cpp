#include <whirl/runtime/runtime.hpp>

#include <whirl/engines/reflect.hpp>

// It is OK!
#include <whirl/engines/matrix/server/server.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::node {

IRuntime& GetRuntime() {
  if (IsMatrix()) {
    return matrix::ThisServer().GetNodeRuntime();
  }
  WHEELS_PANIC("GetRuntime is not supported for current engine");
}

}  // namespace whirl::node

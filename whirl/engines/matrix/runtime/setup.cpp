#include <whirl/engines/matrix/runtime/setup.hpp>

#include <whirl/engines/matrix/server/server.hpp>

#include <whirl/runtime/access.hpp>

namespace whirl::matrix {

static node::IRuntime& GetMatrixRuntime() {
  return ThisServer().GetNodeRuntime();
}

void SetupMatrixRuntime() {
  node::SetupRuntime([]() -> node::IRuntime& {
    return GetMatrixRuntime();
  });
}

}  // namespace whirl::matrix

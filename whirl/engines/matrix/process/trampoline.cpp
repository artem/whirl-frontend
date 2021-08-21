#include <whirl/engines/matrix/process/trampoline.hpp>

#include <whirl/runtime/methods.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl::matrix::process {

void MainTrampoline(node::Program main) {
  auto main_fiber = [main]() {
    main();
  };

  await::fibers::Go(
      main_fiber,
      node::rt::FiberManager(),
      node::rt::Executor());
}

}  // namespace whirl::matrix::process

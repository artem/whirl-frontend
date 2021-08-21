#include <whirl/engines/matrix/process/trampoline.hpp>

#include <whirl/runtime/methods.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl::matrix::process {

static void ServiceMain(node::ProgramMain user_main) {
  user_main();
  // TODO: Stop process
}

void MainTrampoline(node::ProgramMain main) {
  auto main_fiber = [main]() {
    ServiceMain(main);
  };

  await::fibers::Go(             //
      main_fiber,                //
      node::rt::FiberManager(),  //
      node::rt::Executor());
}

}  // namespace whirl::matrix::process

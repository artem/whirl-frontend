#include <whirl/engines/matrix/fault/self.hpp>

#include <whirl/runtime/runtime.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::matrix {

void PauseThisThread() {
  auto& runtime = node::GetRuntime();

  if (!await::fibers::AmIFiber()) {
    return;
  }

  auto pause = TimeModel()->ThreadPause();
  auto after = runtime.TimeService()->After(pause);
  await::fibers::Await(std::move(after)).ExpectOk();
}

void CrashThisServer() {
  // TODO
}

void InjectFault() {
  // TODO
}

}  // namespace whirl::matrix

#include <whirl/engines/matrix/fault/thread.hpp>

#include <whirl/engines/matrix/server/server.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::matrix {

void ThreadPause() {
  auto& runtime = ThisServer().GetNodeRuntime();

  if (!await::fibers::AmIFiber()) {
    return;
  }

  auto pause = TimeModel()->ThreadPause();
  auto after = runtime.TimeService()->After(pause);
  await::fibers::Await(std::move(after)).ExpectOk();
}

}  // namespace whirl::matrix

#include <whirl/engines/matrix/fault/thread.hpp>

#include <whirl/engines/matrix/server/server.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::matrix {

void ThreadPause() {
  const auto& runtime = ThisServer().GetNodeRuntime();

  auto pause = GetTimeModel()->ThreadPause();
  auto after = runtime.time_service->After(pause);
  await::fibers::Await(std::move(after)).ExpectOk();
}

}  // namespace whirl::matrix
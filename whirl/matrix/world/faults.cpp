#include <whirl/matrix/world/faults.hpp>

#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

static Server& AccessServer(size_t index) {
  return WorldImpl::Access()->GetServer(index);
}

static Network& AccessNetwork() {
  return WorldImpl::Access()->GetNetwork();
}

size_t ServerCount() {
  return WorldImpl::Access()->NumServers();
}

std::string GetServerName(size_t index) {
  return AccessServer(index).Name();
}

void PauseServer(size_t index) {
  AccessServer(index).Pause();
}

void ResumeServer(size_t index) {
  AccessServer(index).Resume();
}

void RebootServer(size_t index) {
  AccessServer(index).Reboot();
}

void AdjustServerClock(size_t index) {
  AccessServer(index).AdjustWallTime();
}

}  // namespace whirl

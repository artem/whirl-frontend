#include <whirl/matrix/world/faults.hpp>

#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

static Server& AccessServer(size_t index) {
  return WorldImpl::Access()->GetServer(index);
}

IFaultyServer& AccessFaultyServer(size_t index) {
  return AccessServer(index);
}

size_t ServerCount() {
  return WorldImpl::Access()->NumServers();
}

std::string GetServerName(size_t index) {
  return AccessServer(index).Name();
}

IFaultyNetwork& AccessFaultyNetwork() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl

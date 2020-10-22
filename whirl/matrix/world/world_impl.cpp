#include <whirl/matrix/world/world_impl.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

static WorldImpl* world = nullptr;

WorldImpl::WorldGuard::WorldGuard(WorldImpl* w) {
  world = w;
}

WorldImpl::WorldGuard::~WorldGuard() {
  world = nullptr;
}

WorldImpl* WorldImpl::Access() {
  WHEELS_VERIFY(world != nullptr, "Not in world context");
  return world;
}

}  // namespace whirl

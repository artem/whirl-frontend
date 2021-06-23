#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

static WorldImpl* this_world = nullptr;

WorldImpl::WorldGuard::WorldGuard(WorldImpl* world) {
  this_world = world;
}

WorldImpl::WorldGuard::~WorldGuard() {
  this_world = nullptr;
}

WorldImpl* WorldImpl::Access() {
  WHEELS_VERIFY(this_world != nullptr, "Not in world context");
  return this_world;
}

}  // namespace whirl

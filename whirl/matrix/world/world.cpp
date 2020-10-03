#include <whirl/matrix/world/world.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

static World* world = nullptr;

World::WorldGuard::WorldGuard(World* w) {
  world = w;
}

World::WorldGuard::~WorldGuard() {
  world = nullptr;
}

World* World::Access() {
  WHEELS_VERIFY(world != nullptr, "Not in world context");
  return world;
}

}  // namespace whirl

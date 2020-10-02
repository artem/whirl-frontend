#include <whirl/matrix/world.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

static World* world = nullptr;

World::ScopeGuard::ScopeGuard(World* w) {
  world = w;
}

World::ScopeGuard::~ScopeGuard() {
  world = nullptr;
}

World* World::Access() {
  WHEELS_VERIFY(world != nullptr, "Not in world context");
  return world;
}

}  // namespace whirl

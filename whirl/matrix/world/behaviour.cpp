#include <whirl/matrix/world/behaviours/crazy.hpp>

namespace whirl {

IWorldBehaviourPtr DefaultBehaviour() {
  return std::make_shared<CrazyWorldBehaviour>();
}

}  // namespace whirl

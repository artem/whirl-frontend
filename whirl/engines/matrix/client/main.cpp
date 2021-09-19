#include <whirl/engines/matrix/client/main.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

namespace whirl::matrix::client {

//////////////////////////////////////////////////////////////////////

static void RandomPause() {
  node::rt::SleepFor(node::rt::RandomNumber(50));
}

//////////////////////////////////////////////////////////////////////

void Prologue() {
  await::fibers::self::SetName("main");
  RandomPause();
}

}  // namespace whirl::matrix::client

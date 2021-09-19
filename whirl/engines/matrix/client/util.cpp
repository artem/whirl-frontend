#include <whirl/engines/matrix/client/util.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

namespace whirl::matrix::client {

bool Either() {
  return node::rt::RandomNumber(2) == 1;
}

}  // namespace whirl::matrix::client

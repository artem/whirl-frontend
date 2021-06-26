#include <whirl/node/this.hpp>

// It is OK!
#include <whirl/engines/matrix/server/server.hpp>

namespace whirl {

const NodeServices& ThisNodeServices() {
  return matrix::ThisServer().ThisNodeServices();
}

}  // namespace whirl

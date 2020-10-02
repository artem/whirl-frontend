#pragma once

#include <whirl/matrix/cluster.hpp>

namespace whirl {

// For adversary

struct WorldView {
  Servers& servers;
  Network& network;
};

}  // namespace whirl

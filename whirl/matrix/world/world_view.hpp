#pragma once

#include <whirl/matrix/world/cluster.hpp>
#include <whirl/matrix/network/network.hpp>

namespace whirl {

// For adversary

struct WorldView {
  Servers& servers;
  Network& network;
};

}  // namespace whirl

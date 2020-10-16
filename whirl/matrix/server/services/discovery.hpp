#pragma once

#include <whirl/services/discovery.hpp>

#include <whirl/matrix/world/world.hpp>

namespace whirl {

class DiscoveryService : public IDiscoveryService {
 public:
  std::vector<std::string> GetCluster() override {
    return World::Access()->ClusterAddresses();
  }
};

}  // namespace whirl

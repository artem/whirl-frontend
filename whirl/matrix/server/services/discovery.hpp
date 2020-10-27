#pragma once

#include <whirl/services/discovery.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl {

class DiscoveryService : public IDiscoveryService {
 public:
  size_t GetClusterSize() const override {
    return ClusterSize();
  }

  std::vector<std::string> GetCluster() override {
    return GetClusterAddresses();
  }
};

}  // namespace whirl

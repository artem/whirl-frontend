#pragma once

#include <whirl/services/discovery.hpp>

#include <whirl/matrix/world/global/global.hpp>

namespace whirl {

class DiscoveryService : public IDiscoveryService {
 public:
  size_t GetClusterSize() const override {
    return whirl::GetClusterSize();
  }

  std::vector<std::string> GetCluster() override {
    return whirl::GetCluster();
  }
};

}  // namespace whirl

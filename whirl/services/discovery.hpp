#pragma once

#include <vector>
#include <string>
#include <memory>

namespace whirl {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  using Pool = std::vector<std::string>;

  virtual Pool GetPool(const std::string& name) = 0;

  // Backward compatibility
  Pool GetCluster() {
    return GetPool("cluster");
  }
};

using IDiscoveryPtr = std::shared_ptr<IDiscoveryService>;

}  // namespace whirl

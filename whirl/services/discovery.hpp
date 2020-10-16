#pragma once

#include <vector>
#include <string>
#include <memory>

namespace whirl {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  virtual std::vector<std::string> GetCluster() = 0;
};

using IDiscoveryPtr = std::shared_ptr<IDiscoveryService>;

}  // namespace whirl

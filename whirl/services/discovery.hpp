#pragma once

#include <vector>
#include <string>
#include <memory>

namespace whirl {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  virtual size_t GetClusterSize() const = 0;

  // List addresses of cluster servers
  virtual std::vector<std::string> GetCluster() = 0;
};

using IDiscoveryPtr = std::shared_ptr<IDiscoveryService>;

}  // namespace whirl

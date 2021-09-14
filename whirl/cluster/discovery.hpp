#pragma once

#include <vector>
#include <string>
#include <memory>

namespace whirl::node::cluster {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  // List of hostnames
  using PoolListing = std::vector<std::string>;

  virtual PoolListing ListPool(const std::string& name) = 0;
};

}  // namespace whirl::node::cluster

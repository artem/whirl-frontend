#pragma once

#include <whirl/cluster/discovery.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

namespace whirl::matrix {

class DiscoveryService : public node::cluster::IDiscoveryService {
 public:
  PoolListing ListPool(const std::string& name) override {
    return ::whirl::matrix::GetPool(name);
  }
};

}  // namespace whirl::matrix

#pragma once

#include <whirl/node/cluster/list.hpp>

#include <memory>

namespace whirl::node::cluster {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  // Pool name -> list of hostnames
  virtual List ListPool(const std::string& name) = 0;
};

}  // namespace whirl::node::cluster

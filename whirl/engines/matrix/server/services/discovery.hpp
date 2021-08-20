#pragma once

#include <whirl/services/discovery.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

namespace whirl::matrix {

class DiscoveryService : public IDiscoveryService {
 public:
  std::vector<std::string> ListPool(const std::string& name) override {
    return ::whirl::matrix::GetPool(name);
  }
};

}  // namespace whirl::matrix

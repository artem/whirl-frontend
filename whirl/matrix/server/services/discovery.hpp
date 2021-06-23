#pragma once

#include <whirl/services/discovery.hpp>

#include <whirl/matrix/world/global/global.hpp>

namespace whirl {

class DiscoveryService : public IDiscoveryService {
 public:
  std::vector<std::string> GetPool(const std::string& name) override {
    return whirl::GetPool(name);
  }
};

}  // namespace whirl

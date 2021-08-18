#pragma once

#include <vector>
#include <string>
#include <memory>

namespace whirl {

struct IDiscoveryService {
  virtual ~IDiscoveryService() = default;

  // List of hostnames
  using Pool = std::vector<std::string>;

  virtual Pool GetPool(const std::string& name) = 0;
};

}  // namespace whirl

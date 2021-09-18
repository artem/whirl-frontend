#pragma once

#include <string>
#include <cstdlib>

namespace whirl::matrix {

struct ServerConfig {
  size_t id;
  std::string hostname;
  std::string pool;
};

}  // namespace whirl::matrix

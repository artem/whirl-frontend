#pragma once

#include <string>
#include <cstdlib>

namespace whirl::matrix {

struct ServerConfig {
  size_t id;
  std::string hostname;
};

}  // namespace whirl::matrix

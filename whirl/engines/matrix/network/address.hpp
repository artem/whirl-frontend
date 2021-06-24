#pragma once

#include <cstdlib>
#include <string>

#include <ostream>
#include <fmt/ostream.h>

namespace whirl::matrix::net {

using Port = uint16_t;

struct Address {
  std::string host;
  Port port;
};

std::ostream& operator<<(std::ostream& out, const Address& address);

}  // namespace whirl::matrix::net

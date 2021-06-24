#include <whirl/engines/matrix/network/address.hpp>

namespace whirl::matrix::net {

std::ostream& operator<<(std::ostream& out, const Address& address) {
  out << address.host << ":" << address.port;
  return out;
}

}  // namespace whirl::matrix::net

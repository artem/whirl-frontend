#include <whirl/matrix/network/address.hpp>

namespace whirl::net {

std::ostream& operator<<(std::ostream& out, const Address& address) {
  out << address.host << ":" << address.port;
  return out;
}

}  // namespace whirl::net

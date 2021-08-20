#include <whirl/engines/matrix/fault/accessors.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname) {
  return WorldImpl::Access()->GetServer(hostname);
}

IFaultyNetwork& Network() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl::matrix::fault

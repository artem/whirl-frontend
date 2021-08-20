#include <whirl/engines/matrix/fault/accessors.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(size_t index) {
  return WorldImpl::Access()->GetServer(index);
}

IFaultyNetwork& Network() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl::matrix::fault

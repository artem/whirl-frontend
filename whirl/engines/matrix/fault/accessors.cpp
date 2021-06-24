#include <whirl/engines/matrix/fault/accessors.hpp>

#include <whirl/engines/matrix/world/impl.hpp>

namespace whirl::matrix {

IFaultyServer& AccessFaultyServer(size_t index) {
  return WorldImpl::Access()->GetServer(index);
}

IFaultyNetwork& AccessFaultyNetwork() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl::matrix

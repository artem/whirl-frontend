#include <whirl/matrix/fault/accessors.hpp>

#include <whirl/matrix/world/impl.hpp>

namespace whirl {

IFaultyServer& AccessFaultyServer(size_t index) {
  return WorldImpl::Access()->GetServer(index);
}

IFaultyNetwork& AccessFaultyNetwork() {
  return WorldImpl::Access()->GetNetwork();
}

}  // namespace whirl

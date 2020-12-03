#pragma once

#include <whirl/matrix/fault/server.hpp>
#include <whirl/matrix/fault/network.hpp>

namespace whirl {

IFaultyServer& AccessFaultyServer(size_t index);
IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl

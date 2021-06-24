#pragma once

#include <whirl/engines/matrix/fault/server.hpp>
#include <whirl/engines/matrix/fault/network.hpp>

namespace whirl::matrix {

IFaultyServer& AccessFaultyServer(size_t index);
IFaultyNetwork& AccessFaultyNetwork();

}  // namespace whirl::matrix

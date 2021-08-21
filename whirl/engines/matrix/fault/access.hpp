#pragma once

#include <whirl/engines/matrix/fault/server.hpp>
#include <whirl/engines/matrix/fault/network.hpp>

namespace whirl::matrix::fault {

IFaultyServer& Server(const std::string& hostname);
IFaultyNetwork& Network();

}  // namespace whirl::matrix::fault

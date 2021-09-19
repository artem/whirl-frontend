#pragma once

#include <commute/rpc/server.hpp>

#include <cstdint>

namespace whirl::node::rpc {

commute::rpc::IServerPtr MakeServer(uint16_t port);

}  // namespace whirl::node::pc

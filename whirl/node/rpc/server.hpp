#pragma once

#include <commute/rpc/server.hpp>

#include <cstdint>

namespace whirl::node::rpc {

// Make RPC server on top of node runtime
commute::rpc::IServerPtr MakeServer(uint16_t port);

}  // namespace whirl::node::pc

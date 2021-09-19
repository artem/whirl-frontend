#pragma once

#include <commute/rpc/client.hpp>

namespace whirl::node::rpc {

// Make RPC client on top of node runtime
commute::rpc::IClientPtr MakeClient();

}  // namespace whirl::node::rpc

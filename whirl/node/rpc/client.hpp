#pragma once

#include <commute/rpc/client.hpp>

namespace whirl::node::rpc {

commute::rpc::IClientPtr MakeClient();

}  // namespace whirl::node::rpc

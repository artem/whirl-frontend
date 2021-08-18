#pragma once

#include <whirl/rpc/server.hpp>

namespace whirl::node {

void MainPrologue();

rpc::IServerPtr MakeRPCServer();

void BlockForever();

}  // namespace whirl::node

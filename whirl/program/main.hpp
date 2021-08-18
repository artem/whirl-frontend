#pragma once

#include <whirl/rpc/server.hpp>

namespace whirl::node::main {

void Prologue();

rpc::IServerPtr MakeRPCServer();

[[noreturn]] void BlockForever();

}  // namespace whirl::node::main

#pragma once

#include <whirl/rpc/channel.hpp>

namespace whirl::matrix::client {

void Prologue();

rpc::IChannelPtr MakeRpcChannel();

}  // namespace whirl::matrix::client

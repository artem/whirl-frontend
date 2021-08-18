#pragma once

#include <whirl/rpc/channel.hpp>

namespace whirl::matrix {

void ClientPrologue();

rpc::IChannelPtr MakeClientChannel();

}  // namespace whirl::matrix

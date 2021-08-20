#pragma once

#include <whirl/rpc/channel.hpp>

#include <vector>

namespace whirl::rpc {

IChannelPtr MakeRoundRobinChannel(std::vector<IChannelPtr> channels);

}  // namespace whirl::rpc

#pragma once

#include <whirl/node/rpc/channel.hpp>

#include <whirl/node/random/random.hpp>

#include <vector>

namespace whirl::rpc {

IChannelPtr MakeRandomChannel(std::vector<IChannelPtr>&& channels,
                              node::IRandomService* random);

}  // namespace whirl::rpc

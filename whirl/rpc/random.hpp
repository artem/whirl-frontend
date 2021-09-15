#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/random/random.hpp>

#include <vector>

namespace whirl::rpc {

IChannelPtr MakeRandomChannel(std::vector<IChannelPtr>&& channels,
                              node::IRandomService* random);

}  // namespace whirl::rpc

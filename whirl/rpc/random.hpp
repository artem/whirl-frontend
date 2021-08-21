#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/random.hpp>

#include <vector>

namespace whirl::rpc {

IChannelPtr MakeRandomChannel(std::vector<IChannelPtr>&& channels,
                              IRandomService* random);

}  // namespace whirl::rpc

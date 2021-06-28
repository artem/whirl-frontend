#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/random.hpp>

#include <vector>

namespace whirl::rpc {

rpc::IChannelPtr MakeRandomChannel(std::vector<rpc::IChannelPtr>&& channels,
                                   IRandomServicePtr random);

}  // namespace whirl::rpc

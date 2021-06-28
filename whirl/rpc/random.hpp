#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/random.hpp>

#include <vector>

namespace whirl::matrix {

rpc::IChannelPtr MakeRandomChannel(std::vector<rpc::IChannelPtr>&& channels,
                                   IRandomServicePtr random);

}  // namespace whirl::matrix

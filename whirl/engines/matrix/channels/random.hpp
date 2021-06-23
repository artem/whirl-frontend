#pragma once

#include <whirl/rpc/channel.hpp>

#include <vector>

namespace whirl {

rpc::IChannelPtr MakeRandomChannel(std::vector<rpc::IChannelPtr>&& channels);

}  // namespace whirl

#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <vector>

namespace whirl {

rpc::IChannelPtr MakeRandomChannel(
    std::vector<rpc::IChannelPtr>&& channels);

}  // namespace whirl

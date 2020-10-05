#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <vector>

namespace whirl {

rpc::IRPCChannelPtr MakeRandomChannel(
    std::vector<rpc::IRPCChannelPtr>&& channels);

}  // namespace whirl

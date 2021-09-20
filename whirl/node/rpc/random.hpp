#pragma once

#include <commute/rpc/channel.hpp>

#include <whirl/node/random/service.hpp>

#include <vector>

namespace whirl::rpc {

commute::rpc::IChannelPtr MakeRandomChannel(
    std::vector<commute::rpc::IChannelPtr>&& channels,
    node::random::IRandomService* random);

}  // namespace whirl::rpc

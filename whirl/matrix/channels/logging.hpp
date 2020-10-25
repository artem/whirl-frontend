#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <vector>

namespace whirl {

rpc::IChannelPtr MakeLoggingChannel(rpc::IChannelPtr channel);

}  // namespace whirl

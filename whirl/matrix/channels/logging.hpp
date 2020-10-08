#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <vector>

namespace whirl {

rpc::IRPCChannelPtr MakeLoggingChannel(rpc::IRPCChannelPtr channel);

}  // namespace whirl

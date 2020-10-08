#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <whirl/services/time.hpp>

#include <vector>

namespace whirl {

rpc::IRPCChannelPtr WithRetries(rpc::IRPCChannelPtr channel, ITimeServicePtr time);

}  // namespace whirl

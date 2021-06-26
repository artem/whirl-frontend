#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/time.hpp>

namespace whirl::rpc {

IChannelPtr WithRetries(IChannelPtr channel, ITimeServicePtr time);

}  // namespace whirl::rpc

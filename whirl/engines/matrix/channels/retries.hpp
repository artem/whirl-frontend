#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/time.hpp>

#include <vector>

namespace whirl::matrix {

rpc::IChannelPtr WithRetries(rpc::IChannelPtr channel, ITimeServicePtr time);

}  // namespace whirl::matrix

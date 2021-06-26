#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/time.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

struct BackoffParams {
  Duration init;
  Duration max;
  size_t factor;
};

//////////////////////////////////////////////////////////////////////

IChannelPtr WithRetries(IChannelPtr channel, ITimeServicePtr time,
                        BackoffParams backoff_params);

}  // namespace whirl::rpc

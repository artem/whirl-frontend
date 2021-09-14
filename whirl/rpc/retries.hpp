#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/time/time.hpp>

#include <timber/backend.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

struct BackoffParams {
  Duration init;
  Duration max;
  size_t factor;
};

//////////////////////////////////////////////////////////////////////

IChannelPtr WithRetries(IChannelPtr channel, node::time::ITimeService* time,
                        timber::ILogBackend* log,
                        BackoffParams backoff_params);

}  // namespace whirl::rpc

#pragma once

#include <commute/rpc/channel.hpp>

#include <whirl/node/time/time.hpp>

#include <timber/backend.hpp>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

struct BackoffParams {
  Jiffies init;
  Jiffies max;
  size_t factor;
};

//////////////////////////////////////////////////////////////////////

commute::rpc::IChannelPtr WithRetries(commute::rpc::IChannelPtr channel,
                                      node::time::ITimeService* time,
                                      timber::ILogBackend* log,
                                      BackoffParams backoff_params);

}  // namespace whirl::rpc

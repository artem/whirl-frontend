#pragma once

#include <whirl/rpc/channel.hpp>

namespace whirl {

rpc::IChannelPtr MakeHistoryChannel(rpc::IChannelPtr channel);

}  // namespace whirl

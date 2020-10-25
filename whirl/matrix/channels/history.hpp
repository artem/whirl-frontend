#pragma once

#include <whirl/rpc/impl/channel.hpp>

namespace whirl {

rpc::IChannelPtr MakeHistoryChannel(rpc::IChannelPtr channel);

}  // namespace whirl

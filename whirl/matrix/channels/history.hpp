#pragma once

#include <whirl/rpc/impl/channel.hpp>

namespace whirl {

rpc::IRPCChannelPtr MakeHistoryChannel(rpc::IRPCChannelPtr channel);

}  // namespace whirl

#pragma once

#include <whirl/node/rpc/channel.hpp>

namespace whirl::matrix {

rpc::IChannelPtr MakeHistoryChannel(rpc::IChannelPtr channel);

}  // namespace whirl::matrix

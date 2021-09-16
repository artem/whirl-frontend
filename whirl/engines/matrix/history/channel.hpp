#pragma once

#include <commute/rpc/channel.hpp>

namespace whirl::matrix {

commute::rpc::IChannelPtr MakeHistoryChannel(commute::rpc::IChannelPtr channel);

}  // namespace whirl::matrix

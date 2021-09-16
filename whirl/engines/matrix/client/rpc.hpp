#pragma once

#include <commute/rpc/channel.hpp>

namespace whirl::matrix::client {

commute::rpc::IChannelPtr MakeRpcChannel(const std::string& pool_name, uint16_t port);

}  // namespace whirl::matrix::client

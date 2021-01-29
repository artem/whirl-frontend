#pragma once

#include <whirl/rpc/channel.hpp>

#include <whirl/services/executor.hpp>
#include <whirl/rpc/net_transport.hpp>

#include <memory>

namespace whirl::rpc {

struct IClient {
  virtual ~IClient() = default;

  virtual IChannelPtr Dial(const std::string& peer) = 0;
};

using IClientPtr = std::shared_ptr<IClient>;

IClientPtr MakeClient(ITransportPtr t, IExecutorPtr e);

}  // namespace whirl::rpc

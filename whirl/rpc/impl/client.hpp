#pragma once

#include <whirl/rpc/impl/channel.hpp>

#include <whirl/services/executor.hpp>
#include <whirl/rpc/impl/net_transport.hpp>

#include <memory>

namespace whirl::rpc {

struct IClient {
  virtual ~IClient() = default;

  virtual IChannelPtr MakeChannel(const std::string& peer) = 0;
};

using IClientPtr = std::shared_ptr<IClient>;

IClientPtr MakeClient(ITransportPtr t, IExecutorPtr e);

}  // namespace whirl::rpc

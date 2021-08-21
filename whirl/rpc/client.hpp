#pragma once

#include <whirl/services/net_transport.hpp>

#include <whirl/rpc/channel.hpp>

#include <await/executors/executor.hpp>

#include <memory>

namespace whirl::rpc {

struct IClient {
  virtual ~IClient() = default;

  virtual IChannelPtr Dial(const std::string& peer) = 0;
};

using IClientPtr = std::shared_ptr<IClient>;

IClientPtr MakeClient(ITransport* t, await::executors::IExecutorPtr e);

}  // namespace whirl::rpc

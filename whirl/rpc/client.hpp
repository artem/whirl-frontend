#pragma once

#include <whirl/net/transport.hpp>

#include <whirl/rpc/channel.hpp>

#include <await/executors/executor.hpp>

#include <timber/backend.hpp>

#include <memory>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

struct IClient {
  virtual ~IClient() = default;

  virtual IChannelPtr Dial(const std::string& peer) = 0;
};

using IClientPtr = std::shared_ptr<IClient>;

//////////////////////////////////////////////////////////////////////

IClientPtr MakeClient(node::net::ITransport* t, await::executors::IExecutor* e,
                      timber::ILogBackend* log);

}  // namespace whirl::rpc

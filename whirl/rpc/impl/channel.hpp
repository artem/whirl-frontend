#pragma once

#include <whirl/rpc/impl/method.hpp>
#include <whirl/rpc/impl/raw_value.hpp>

#include <await/futures/future.hpp>

#include <memory>

namespace whirl::rpc {

using await::futures::Future;

//////////////////////////////////////////////////////////////////////

// Communication line between client and remote service

struct IChannel {
  virtual ~IChannel() = default;

  virtual Future<BytesValue> Call(const Method& method,
                                  const BytesValue& input) = 0;

  virtual void Close() = 0;

  virtual const std::string& Peer() const = 0;
};

using IChannelPtr = std::shared_ptr<IChannel>;

//////////////////////////////////////////////////////////////////////

struct IRPCClient {
  virtual ~IRPCClient() = default;

  virtual IChannelPtr MakeChannel(std::string peer) = 0;
};

using IRPCClientPtr = std::shared_ptr<IRPCClient>;

}  // namespace whirl::rpc

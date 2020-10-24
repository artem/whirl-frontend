#pragma once

#include <whirl/rpc/impl/callee.hpp>
#include <whirl/rpc/impl/raw_value.hpp>

#include <await/futures/future.hpp>

#include <memory>

namespace whirl::rpc {

using await::futures::Future;

//////////////////////////////////////////////////////////////////////

// Communication line between client and remote service

struct IRPCChannel {
  virtual ~IRPCChannel() = default;

  virtual void Start() = 0;

  virtual Future<BytesValue> Call(const Callee& callee,
                                  const BytesValue& input) = 0;

  virtual void Close() = 0;

  virtual const std::string& Peer() const = 0;
};

using IRPCChannelPtr = std::shared_ptr<IRPCChannel>;

//////////////////////////////////////////////////////////////////////

struct IRPCClient {
  virtual ~IRPCClient() = default;

  virtual IRPCChannelPtr MakeChannel(std::string peer) = 0;
};

using IRPCClientPtr = std::shared_ptr<IRPCClient>;

}  // namespace whirl::rpc

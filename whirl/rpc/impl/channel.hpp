#pragma once

#include <whirl/rpc/impl/raw.hpp>

#include <await/futures/future.hpp>

#include <memory>

namespace whirl::rpc {

using await::futures::Future;

struct IRPCChannel {
  virtual ~IRPCChannel() = default;

  virtual void Start() = 0;
  virtual Future<RPCBytes> Call(const std::string& method,
                                const RPCBytes& input) = 0;
  virtual void Close() = 0;

  virtual const std::string& Peer() const = 0;
};

using IRPCChannelPtr = std::shared_ptr<IRPCChannel>;

struct IRPCClient {
  virtual ~IRPCClient() = default;

  virtual IRPCChannelPtr Dial(std::string peer) = 0;
};

using IRPCClientPtr = std::shared_ptr<IRPCClient>;

}  // namespace whirl::rpc

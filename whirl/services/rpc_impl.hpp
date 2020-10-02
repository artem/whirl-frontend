#pragma once

#include <await/futures/future.hpp>

#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Serialized input/result values
using RPCBytes = std::string;

//////////////////////////////////////////////////////////////////////

using RPCMethodInvoker = std::function<RPCBytes(const RPCBytes)>;

//////////////////////////////////////////////////////////////////////

struct IRPCServerImpl {
  virtual ~IRPCServerImpl() = default;

  virtual void RegisterMethod(const std::string& method,
                              RPCMethodInvoker invoker) = 0;
};

using IRPCServerImplPtr = std::shared_ptr<IRPCServerImpl>;

//////////////////////////////////////////////////////////////////////

struct IRPCChannelImpl {
  virtual ~IRPCChannelImpl() = default;

  virtual Future<RPCBytes> Call(const std::string& method,
                                const RPCBytes& input) = 0;

  virtual const std::string& Peer() const = 0;

  virtual void Shutdown() = 0;
};

using IRPCChannelImplPtr = std::shared_ptr<IRPCChannelImpl>;

//////////////////////////////////////////////////////////////////////

struct IRPCClientImpl {
  virtual ~IRPCClientImpl() = default;

  virtual IRPCChannelImplPtr Dial(const std::string& peer) = 0;
};

using IRPCClientImplPtr = std::shared_ptr<IRPCClientImpl>;

}  // namespace whirl

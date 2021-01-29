#pragma once

#include <whirl/rpc/method.hpp>
#include <whirl/rpc/bytes_value.hpp>

#include <await/futures/future.hpp>

#include <memory>

namespace whirl::rpc {

using await::futures::Future;

// Communication line between client and remote service

struct IChannel {
  virtual ~IChannel() = default;

  // Unary RPC call
  virtual Future<BytesValue> Call(const Method& method,
                                  const BytesValue& input) = 0;

  virtual void Close() = 0;

  virtual const std::string& Peer() const = 0;
};

using IChannelPtr = std::shared_ptr<IChannel>;

}  // namespace whirl::rpc

#pragma once

#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/net_transport.hpp>
#include <await/executors/executor.hpp>

namespace whirl::rpc {

using await::executors::IExecutorPtr;

class TClient {
 public:
  TClient() {
  }

  TClient(ITransportPtr t, IExecutorPtr e)
      : transport_(std::move(t)), executor_(std::move(e)) {
  }

  IChannelPtr MakeChannel(const std::string& peer);

 private:
  ITransportPtr transport_;
  IExecutorPtr executor_;
};

}  // namespace whirl::rpc

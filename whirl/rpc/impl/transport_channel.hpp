#pragma once

#include <whirl/rpc/impl/method.hpp>
#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/trace.hpp>
#include <whirl/rpc/impl/net_transport.hpp>

#include <await/executors/executor.hpp>
#include <await/executors/strand.hpp>

#include <await/futures/promise.hpp>

// TODO: abstract logger
#include <whirl/matrix/log/logger.hpp>

#include <map>

namespace whirl::rpc {

using await::executors::IExecutorPtr;
using await::futures::Promise;

//////////////////////////////////////////////////////////////////////

// Fair-loss channel

class TransportChannel : public std::enable_shared_from_this<TransportChannel>,
                         public IChannel,
                         public ITransportHandler {
 private:
  struct Request {
    RequestId id;
    TraceId trace_id;
    Method method;
    BytesValue input;
    Promise<BytesValue> promise;
  };

  using Requests = std::map<RequestId, Request>;

 public:
  TransportChannel(ITransportPtr t, IExecutorPtr e, TransportAddress peer)
      : transport_(std::move(t)),
        executor_(e),
        peer_(peer),
        strand_(await::executors::MakeStrand(e)) {
  }

  void Start() {
    // GetTransportSocket();
  }

  ~TransportChannel() {
    Close();
  }

  // IChannel

  Future<BytesValue> Call(const Method& method,
                          const BytesValue& input) override;

  void Close() override;

  const std::string& Peer() const override {
    return peer_;
  }

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr /*back*/) override {
    strand_->Execute([self = shared_from_this(), message]() {
      self->ProcessResponse(message);
    });
  }

  void HandleDisconnect(const std::string& /*peer*/) override {
    strand_->Execute([self = shared_from_this()]() { self->LostPeer(); });
  }

 private:
  Request MakeRequest(const Method& method, const BytesValue& input);

  // Inside strand executor
  void SendRequest(Request request);
  void ProcessResponse(const TransportMessage& message);
  void LostPeer();
  void DoClose();

  ResponseMessage ParseResponse(const TransportMessage& message);

 private:
  ITransportSocketPtr& GetTransportSocket();

  void Fail(Request& request, std::error_code e);

 private:
  ITransportPtr transport_;
  IExecutorPtr executor_;  // For callbacks

  const TransportAddress peer_;

  IExecutorPtr strand_;
  // State guarded by strand_
  ITransportSocketPtr socket_{nullptr};
  Requests requests_;

  Logger logger_{"RPC channel"};
};

}  // namespace whirl::rpc

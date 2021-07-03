#pragma once

#include <whirl/services/net_transport.hpp>

#include <whirl/rpc/method.hpp>
#include <whirl/rpc/id.hpp>
#include <whirl/rpc/protocol.hpp>
#include <whirl/rpc/channel.hpp>

#include <whirl/logger/log.hpp>

#include <await/executors/executor.hpp>
#include <await/executors/strand.hpp>

#include <await/futures/util/promise.hpp>

#include <map>

namespace whirl::rpc {

//////////////////////////////////////////////////////////////////////

// Fair-loss channel

class TransportChannel : public std::enable_shared_from_this<TransportChannel>,
                         public IChannel,
                         public ITransportHandler {
 private:
  struct ActiveRequest {
    RequestId id;
    TraceId trace_id;
    Method method;
    BytesValue input;
    await::futures::LazyPromise<BytesValue> promise;
  };

  using ActiveRequests = std::map<RequestId, ActiveRequest>;

 public:
  TransportChannel(ITransportPtr t, await::executors::IExecutorPtr e, TransportAddress peer)
      : transport_(std::move(t)),
        executor_(e),
        peer_(peer),
        strand_(await::executors::MakeStrand(e)) {
  }

  void Start() {
    // GetTransportSocket();
  }

  ~TransportChannel();

  // IChannel

  await::futures::Future<BytesValue> Call(const Method& method,
                                          const BytesValue& input,
                                          CallContext ctx) override;

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
    strand_->Execute([self = shared_from_this()]() {
      self->LostPeer();
    });
  }

 private:
  ActiveRequest MakeRequest(const Method& method, const BytesValue& input,
                      const CallContext& ctx);

  // Inside strand executor
  void SendRequest(ActiveRequest request);
  void ProcessResponse(const TransportMessage& message);
  void LostPeer();
  void DoClose();

  proto::Response ParseResponse(const TransportMessage& message);

 private:
  ITransportSocketPtr& GetTransportSocket();

  void Fail(ActiveRequest& request, std::error_code e);

 private:
  ITransportPtr transport_;
  await::executors::IExecutorPtr executor_;  // For callbacks

  const TransportAddress peer_;

  IExecutorPtr strand_;
  // State guarded by strand_
  ITransportSocketPtr socket_{nullptr};
  ActiveRequests requests_;

  Logger logger_{"RPC-Channel"};
};

}  // namespace whirl::rpc

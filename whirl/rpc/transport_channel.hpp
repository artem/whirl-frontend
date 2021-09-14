#pragma once

#include <whirl/services/net_transport.hpp>

#include <whirl/rpc/method.hpp>
#include <whirl/rpc/id.hpp>
#include <whirl/rpc/protocol.hpp>
#include <whirl/rpc/channel.hpp>

#include <await/executors/execute.hpp>
#include <await/executors/strand.hpp>

#include <await/futures/util/promise.hpp>

#include <timber/logger.hpp>

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
  TransportChannel(ITransport* t, await::executors::IExecutor* e,
                   timber::ILogBackend* log, TransportAddress peer)
      : transport_(std::move(t)),
        executor_(e),
        peer_(peer),
        strand_(e),
        logger_("RPC-Channel", log) {
  }

  void Start() {
    // GetTransportSocket();
  }

  ~TransportChannel();

  // IChannel

  await::futures::Future<BytesValue> Call(const Method& method,
                                          const BytesValue& input,
                                          CallOptions ctx) override;

  void Close() override;

  const std::string& Peer() const override {
    return peer_;
  }

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr /*back*/) override {
    await::executors::Execute(strand_, [self = shared_from_this(), message]() {
      self->ProcessResponse(message);
    });
  }

  void HandleDisconnect(const std::string& /*peer*/) override {
    await::executors::Execute(strand_, [self = shared_from_this()]() {
      self->LostPeer();
    });
  }

 private:
  ActiveRequest MakeRequest(const Method& method, const BytesValue& input,
                            const CallOptions& options);

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
  ITransport* transport_;
  await::executors::IExecutor* executor_;  // For callbacks

  const TransportAddress peer_;

  await::executors::Strand strand_;
  // State guarded by strand_
  ITransportSocketPtr socket_{nullptr};
  ActiveRequests requests_;

  timber::Logger logger_;
};

}  // namespace whirl::rpc

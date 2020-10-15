#pragma once

#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/channel.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <whirl/rpc/impl/net_transport.hpp>

#include <await/executors/executor.hpp>
#include <await/executors/strand.hpp>

#include <await/fibers/sync/teleport.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

// TODO: abstract logger
#include <whirl/matrix/log/log.hpp>

#include <map>

namespace whirl::rpc {

using await::executors::IExecutorPtr;
using await::futures::Promise;

//////////////////////////////////////////////////////////////////////

// Fair-loss channel

class RPCTransportChannel
    : public std::enable_shared_from_this<RPCTransportChannel>,
      public IRPCChannel,
      public ITransportHandler {
 private:
  struct Request {
    RPCId id;
    TraceId trace_id;
    std::string method;
    BytesValue input;
    Promise<BytesValue> promise;
  };

  using Requests = std::map<RPCId, Request>;

 public:
  RPCTransportChannel(ITransportPtr t, IExecutorPtr e, std::string peer)
      : transport_(std::move(t)),
        executor_(e),
        strand_(await::executors::MakeStrand(std::move(e))),
        peer_(peer) {
  }

  ~RPCTransportChannel() {
    Close();
  }

  // IRPCChannel

  void Start() override {
    // GetTransportSocket();
  }

  Future<BytesValue> Call(const std::string& method,
                          const BytesValue& input) override;

  void Close() override {
    // TODO: fiber-oblivious
    await::fibers::TeleportGuard t(strand_);

    if (socket_ && socket_->IsConnected()) {
      socket_->Close();
    }
  }

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

  void HandleDisconnect() override {
    strand_->Execute([self = shared_from_this()]() { self->LostPeer(); });
  }

 private:
  Request MakeRequest(const std::string& method, const BytesValue& input);

  // Inside strand executor
  void SendRequest(Request request);
  void ProcessResponse(const TransportMessage& message);
  void LostPeer();

  RPCResponseMessage ParseResponse(const TransportMessage& message);

 private:
  ITransportSocketPtr& GetTransportSocket();

  void Fail(Request& request, std::error_code e) {
    WHIRL_FMT_LOG("Fail request with id = {}", request.id);
    std::move(request.promise).SetError(wheels::Error(e));
  }

 private:
  ITransportPtr transport_;
  IExecutorPtr executor_;  // For callbacks
  IExecutorPtr strand_;

  const std::string peer_;

  ITransportSocketPtr socket_{nullptr};

  Requests requests_;

  Logger logger_{"RPC channel"};
};

}  // namespace whirl::rpc

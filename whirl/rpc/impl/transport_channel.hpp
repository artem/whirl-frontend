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
                          const BytesValue& input) override {

    auto request = MakeRequest(method, input);

    auto future = request.promise.MakeFuture();

    strand_->Execute([self = shared_from_this(), request = std::move(request)]() mutable {
      self->SendRequest(std::move(request));
    });

    return future;
  }

  void Close() override {
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
      self->HandleResponse(message);
    });
  }

  void HandleLostPeer() override {
    strand_->Execute([self = shared_from_this()]() { self->LostPeer(); });
  }

 private:
  // Inside strand executor

  Request MakeRequest(const std::string& method, const BytesValue& input) {
    Request request;

    request.id = GenerateRequestId();
    request.trace_id = GetOrGenerateNewTraceId(request.id);
    request.method = method;
    request.input = input;

    return request;
  }

  void SendRequest(Request request) {
    WHIRL_FMT_LOG("Request method '{}' on peer {}", request.method, peer_);

    ITransportSocketPtr& socket = GetTransportSocket();

    if (!socket->IsConnected()) {
      // Fail RPC immediately
      Fail(request, make_error_code(RPCErrorCode::TransportError));
      return;
    }

    auto id = request.id;

    socket->Send(Serialize<RPCRequestMessage>(
        {request.id, request.trace_id, peer_, request.method, request.input}));

    requests_.emplace(id, std::move(request));
  }

  void HandleResponse(const TransportMessage& message) {
    WHIRL_FMT_LOG("Process response message from {}", peer_);

    auto response = Deserialize<RPCResponseMessage>(message);

    auto request_it = requests_.find(response.request_id);

    if (request_it == requests_.end()) {
      WHIRL_FMT_LOG("Request with id {} not found", response.request_id);
      return;  // Probably duplicated response message from transport layer?
    }

    Request request = std::move(request_it->second);
    requests_.erase(request_it);

    if (response.IsOk()) {
      WHIRL_FMT_LOG("Request with id {} completed", response.request_id);
      std::move(request.promise).SetValue(response.result);
    } else {
      // TODO: better error
      Fail(request, make_error_code(response.error));
    }
  }

  void LostPeer() {
    auto requests = std::move(requests_);
    requests_.clear();

    WHIRL_FMT_LOG(
        "Transport connection to peer {} lost, fail {} pending request(s)",
        peer_, requests.size());

    // Next Call triggers reconnect
    socket_.reset();

    for (auto& [id, request] : requests) {
      Fail(request, make_error_code(RPCErrorCode::TransportError));
    }
  }

 private:
  ITransportSocketPtr& GetTransportSocket() {
    if (socket_) {
      return socket_;
    }
    WHIRL_FMT_LOG("Reconnect to {}", peer_);
    socket_ = transport_->ConnectTo(peer_, shared_from_this());
    return socket_;
  }

  void Fail(Request& request, std::error_code e) {
    WHIRL_FMT_LOG("Fail request with id = {}", request.id);
    std::move(request.promise).SetError(wheels::Error(e));
  }

 private:
  ITransportPtr transport_;
  IExecutorPtr strand_;

  const std::string peer_;

  ITransportSocketPtr socket_{nullptr};

  Requests requests_;

  Logger logger_{"RPC channel"};
};

}  // namespace whirl::rpc

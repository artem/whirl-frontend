#pragma once

#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/channel.hpp>

#include <whirl/services/net_transport.hpp>

#include <await/executors/executor.hpp>
#include <await/executors/strand.hpp>

#include <await/fibers/sync/teleport.hpp>

#include <await/futures/promise.hpp>
#include <await/futures/helpers.hpp>

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
  using RequestPromise = Promise<RPCBytes>;

  struct Request {
    RPCId id;
    RequestPromise promise;
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
    //GetTransportSocket();
  }

  Future<RPCBytes> Call(const std::string& method,
                        const RPCBytes& input) override {

    await::fibers::TeleportGuard t(strand_);

    WHIRL_FMT_LOG("Request method '{}' on peer {}", method, peer_);

    ITransportSocketPtr& socket = GetTransportSocket();

    if (!socket->IsConnected()) {
      // Fail RPC immediately
      return await::futures::MakeError(
          std::make_error_code(std::errc::connection_refused));
    }

    Request request;
    request.id = GenerateRequestId();
    auto future = request.promise.MakeFuture();
    requests_.emplace(request.id, std::move(request));
    socket->Send(
        Serialize<RPCRequestMessage>({request.id, peer_, method, input}));
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
    strand_->Execute([self = shared_from_this()]() {
      self->LostPeer();
    });
  }

 private:
  // Inside strand executor

  void HandleResponse(const TransportMessage& message) {
    WHIRL_FMT_LOG("Process response message from {}", peer_);

    auto response = Deserialize<RPCResponseMessage>(message);

    auto request_it = requests_.find(response.request_id);

    if (request_it == requests_.end()) {
      return;  // Probably duplicated response message from transport layer?
    }

    Request request = std::move(request_it->second);
    requests_.erase(request_it);

    if (response.IsOk()) {
      std::move(request.promise).SetValue(response.result);
    } else {
      // TODO: better error
      Fail(request, std::errc::io_error);
    }
  }

  void LostPeer() {
    auto requests = std::move(requests_);
    requests_.clear();

    WHIRL_FMT_LOG("Transport connection to peer {} lost, fail {} pending request(s)", peer_, requests.size());

    // Next Call triggers reconnect
    socket_.reset();

    for (auto& [id, request] : requests) {
      Fail(request, std::errc::connection_reset);
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

  void Fail(Request& request, std::errc e) {
    WHIRL_FMT_LOG("Fail request with id = {}", request.id);
    std::move(request.promise).SetError(std::make_error_code(e));
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

#pragma once

#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/channel.hpp>

#include <whirl/services/net_transport.hpp>

#include <await/executors/executor.hpp>
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

 public:
  RPCTransportChannel(ITransportPtr t, IExecutorPtr e, std::string peer)
      : transport_(std::move(t)), executor_(std::move(e)), peer_(peer) {
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
    WHIRL_LOG("Request method '" << method << "' on peer " << peer_);

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
    if (socket_) {
      socket_->Close();
    }
  }

  const std::string& Peer() const override {
    return peer_;
  }

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr /*back*/) override {
    // TODO: switch to executor?

    WHIRL_LOG("Message received");

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

  void HandleLostPeer() override {
    // TODO: switch to executor?

    WHIRL_FMT_LOG("Transport connection to {} lost, fail all pending requests", peer_);
    for (auto& [id, request] : requests_) {
      Fail(request, std::errc::connection_reset);
    }
    requests_.clear();
    socket_.reset();
  }

 private:
  ITransportSocketPtr& GetTransportSocket() {
    if (socket_ && socket_->IsConnected()) {
      return socket_;
    }
    WHIRL_LOG("Reconnect to " << peer_);
    socket_ = transport_->ConnectTo(peer_, shared_from_this());
    return socket_;
  }

  void Fail(Request& request, std::errc e) {
    std::move(request.promise).SetError(std::make_error_code(e));
  }

 private:
  ITransportPtr transport_;
  IExecutorPtr executor_;

  std::string peer_;

  ITransportSocketPtr socket_{nullptr};

  std::map<RPCId, Request> requests_;

  Logger logger_{"RPC channel"};
};

}  // namespace whirl::rpc

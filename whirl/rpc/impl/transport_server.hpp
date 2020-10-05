#pragma once

#include <whirl/rpc/impl/raw.hpp>
#include <whirl/rpc/impl/protocol.hpp>

#include <whirl/rpc/impl/net_transport.hpp>

#include <await/executors/executor.hpp>
#include <await/fibers/core/api.hpp>

#include <wheels/support/exception.hpp>

// TODO: abstract logger
#include <whirl/matrix/log/log.hpp>

#include <functional>
#include <memory>
#include <map>

namespace whirl::rpc {

using await::executors::IExecutorPtr;

// Server

//////////////////////////////////////////////////////////////////////

using RPCMethodInvoker = std::function<RPCBytes(const RPCBytes)>;

//////////////////////////////////////////////////////////////////////

class RPCTransportServer
    : public std::enable_shared_from_this<RPCTransportServer>,
      public ITransportHandler {
 public:
  RPCTransportServer(ITransportPtr transport, IExecutorPtr executor)
      : transport_(transport), executor_(executor) {
  }

  void Start() {
    server_ = transport_->Serve(shared_from_this());
  }

  void RegisterMethod(const std::string& method, RPCMethodInvoker invoker) {
    if (methods_.find(method) != methods_.end()) {
      WHEELS_PANIC("RPC method '" << method << "' already registered");
    }

    methods_.emplace(method, std::move(invoker));
  }

  void Shutdown() {
    if (server_) {
      server_->Shutdown();
    }
  }

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr back) override {
    // Process request
    await::fibers::Spawn(
        [this, message, back = std::move(back)]() mutable {
          ProcessRequest(message, std::move(back));
        },
        executor_);
  }

  void HandleLostPeer() override {
    // Some client lost
  }

 private:
  // In separate fiber
  void ProcessRequest(const TransportMessage& message,
                      ITransportSocketPtr back) {
    auto request = Deserialize<RPCRequestMessage>(message);

    WHIRL_LOG("Processing request");

    if (methods_.count(request.method) == 0) {
      // Requested method not found
      ResponseWithError(request, back, {1, "Method not found"});
    }
    RPCBytes result;
    auto invoker = methods_[request.method];
    try {
      result = invoker(request.input);
    } catch (...) {
      ResponseWithError(request, back, {1, wheels::CurrentExceptionMessage()});
    }
    SendResponse({request.id, request.method, result, RPCError::Ok()}, back);
  }

  void ResponseWithError(const RPCRequestMessage& request,
                         ITransportSocketPtr& back, RPCError error) {
    SendResponse({request.id, request.method, "", error}, back);
  }

  void SendResponse(RPCResponseMessage response, ITransportSocketPtr& back) {
    back->Send(Serialize(response));
  }

 private:
  // Services
  ITransportPtr transport_;
  IExecutorPtr executor_;

  ITransportServerPtr server_;

  std::map<std::string, RPCMethodInvoker> methods_;

  Logger logger_{"RPC server"};
};

}  // namespace whirl::rpc

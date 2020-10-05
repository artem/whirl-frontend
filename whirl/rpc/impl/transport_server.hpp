#pragma once

#include <whirl/rpc/impl/raw.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/net_transport.hpp>
#include <whirl/rpc/impl/trace.hpp>

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

using RPCMethodInvoker = std::function<BytesValue(const BytesValue)>;

//////////////////////////////////////////////////////////////////////

class RPCTransportServer
    : public std::enable_shared_from_this<RPCTransportServer>,
      public ITransportHandler {
 public:
  RPCTransportServer(ITransportPtr t, IExecutorPtr e)
      : transport_(t), executor_(e) {
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
        [self = shared_from_this(), message, back = std::move(back)]() mutable {
          self->ProcessRequest(message, back);
        },
        executor_);
  }

  void HandleLostPeer() override {
    // Some client lost
  }

 private:
  // In separate fiber
  void ProcessRequest(const TransportMessage& message,
                      const ITransportSocketPtr& back) {
    auto request = Deserialize<RPCRequestMessage>(message);

    SetThisHandlerTraceId(request.trace_id);

    WHIRL_FMT_LOG("Process method '{}' request with id = {}", request.method,
                  request.id);

    if (methods_.count(request.method) == 0) {
      ResponseWithError(request, back, RPCErrorCode::MethodNotFound);
    }

    BytesValue result;
    auto invoker = methods_[request.method];
    try {
      result = invoker(request.input);
    } catch (...) {
      WHIRL_FMT_LOG("Exception in method '{}': {}", request.method,
                    wheels::CurrentExceptionMessage());
      ResponseWithError(request, back, RPCErrorCode::ExecutionError);
    }
    SendResponse({request.id, request.method, result, RPCErrorCode::Ok}, back);
  }

  void ResponseWithError(const RPCRequestMessage& request,
                         const ITransportSocketPtr& back, RPCErrorCode error) {
    SendResponse({request.id, request.method, "", error}, back);
  }

  void SendResponse(RPCResponseMessage response,
                    const ITransportSocketPtr& back) {
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

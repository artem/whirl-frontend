#include <whirl/rpc/impl/transport_server.hpp>

#include <whirl/rpc/impl/trace.hpp>

#include <await/fibers/core/api.hpp>

#include <wheels/support/exception.hpp>

namespace whirl::rpc {

void RPCTransportServer::Start() {
  server_ = transport_->Serve(shared_from_this());
}

void RPCTransportServer::RegisterMethod(const std::string& method,
                                        RPCMethodInvoker invoker) {
  if (methods_.find(method) != methods_.end()) {
    WHEELS_PANIC("RPC method '" << method << "' already registered");
  }

  methods_.emplace(method, std::move(invoker));
}

void RPCTransportServer::Shutdown() {
  if (server_) {
    server_->Shutdown();
  }
}

// ITransportHandler

void RPCTransportServer::HandleMessage(const TransportMessage& message,
                                       ITransportSocketPtr back) {
  // Process request
  await::fibers::Spawn(
      [self = shared_from_this(), message, back = std::move(back)]() mutable {
        self->ProcessRequest(message, back);
      },
      executor_);
}

void RPCTransportServer::HandleDisconnect() {
  // Some client lost
}

void RPCTransportServer::ProcessRequest(const TransportMessage& message,
                                        const ITransportSocketPtr& back) {
  auto request = Deserialize<RPCRequestMessage>(message);

  SetThisFiberTraceId(request.trace_id);

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

void RPCTransportServer::ResponseWithError(const RPCRequestMessage& request,
                                           const ITransportSocketPtr& back,
                                           RPCErrorCode error) {
  SendResponse({request.id, request.method, "", error}, back);
}

void RPCTransportServer::SendResponse(RPCResponseMessage response,
                                      const ITransportSocketPtr& back) {
  back->Send(Serialize(response));
}

}  // namespace whirl::rpc

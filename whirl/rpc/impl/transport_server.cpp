#include <whirl/rpc/impl/transport_server.hpp>

#include <whirl/rpc/impl/trace.hpp>

#include <await/fibers/core/api.hpp>

#include <wheels/support/exception.hpp>

namespace whirl::rpc {

void RPCTransportServer::Start() {
  server_ = transport_->Serve(shared_from_this());
}

void RPCTransportServer::RegisterService(const std::string& name,
                                         IRPCServicePtr service) {
  if (services_.find(name) != services_.end()) {
    WHEELS_PANIC("RPC service '" << name << "' already registered");
  }

  service->Initialize();
  services_.emplace(name, std::move(service));
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

  WHIRL_FMT_LOG("Process {} request, id = {}", request.method,
                request.id);

  auto service_it = services_.find(request.method.service);

  if (service_it == services_.end()) {
    RespondWithError(request, back, RPCErrorCode::ServiceNotFound);
    return;
  }

  const IRPCServicePtr& service = service_it->second;

  if (!service->Has(request.method.name)) {
    RespondWithError(request, back, RPCErrorCode::MethodNotFound);
    return;
  }

  BytesValue result;
  try {
    result = service->Invoke(request.method.name, request.input);
  } catch (RPCBadRequest& e) {
    RespondWithError(request, back, RPCErrorCode::BadRequest);
    return;
  } catch (...) {
    WHIRL_FMT_LOG("Exception in {}: {}", request.method,
                  wheels::CurrentExceptionMessage());
    RespondWithError(request, back, RPCErrorCode::ExecutionError);
    return;
  }

  // Ok
  SendResponse({request.id, request.method, result, RPCErrorCode::Ok}, back);
}

void RPCTransportServer::RespondWithError(const RPCRequestMessage& request,
                                          const ITransportSocketPtr& back,
                                          RPCErrorCode error) {
  SendResponse({request.id, request.method, "", error}, back);
}

void RPCTransportServer::SendResponse(RPCResponseMessage response,
                                      const ITransportSocketPtr& back) {
  back->Send(Serialize(response));
}

}  // namespace whirl::rpc

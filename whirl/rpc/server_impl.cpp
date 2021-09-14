#include <whirl/rpc/server_impl.hpp>

#include <whirl/rpc/trace.hpp>
#include <whirl/rpc/exceptions.hpp>

#include <whirl/cereal/serialize.hpp>

#include <wheels/support/exception.hpp>
#include <wheels/support/panic.hpp>

#include <await/context/context.hpp>

#include <timber/log.hpp>

namespace whirl::rpc {

void ServerImpl::Start() {
  server_ = transport_->Serve(weak_from_this());
}

void ServerImpl::RegisterService(const std::string& name, IServicePtr service) {
  if (services_.find(name) != services_.end()) {
    WHEELS_PANIC("RPC service '" << name << "' already registered");
  }

  service->Initialize();
  services_.emplace(name, std::move(service));
}

void ServerImpl::Shutdown() {
  if (server_) {
    server_->Shutdown();
  }
  handlers_.Cancel();
  handlers_.Join();
}

// ITransportHandler

void ServerImpl::HandleMessage(const TransportMessage& message,
                               ITransportSocketPtr back) {
  // Process request
  handlers_.Spawn(
      [self = shared_from_this(), message, back = std::move(back)]() mutable {
        self->ProcessRequest(message, back);
      });
}

void ServerImpl::HandleDisconnect(const std::string& /*client*/) {
  // Client disconnected
}

void ServerImpl::ProcessRequest(const TransportMessage& message,
                                const ITransportSocketPtr& back) {
  auto request = Deserialize<proto::Request>(message);

  auto trace = MakeTraceContext(request.trace_id);
  await::context::Scope scope{trace};

  await::fibers::self::SetContext(scope.GetContext());

  LOG_INFO("Process {} request from {}, id = {}", request.method,
                 back->Peer(), request.id);

  auto service_it = services_.find(request.method.service);

  if (service_it == services_.end()) {
    RespondWithError(request, back, RPCErrorCode::ServiceNotFound);
    return;
  }

  const IServicePtr service = service_it->second;

  if (!service->Has(request.method.name)) {
    RespondWithError(request, back, RPCErrorCode::MethodNotFound);
    return;
  }

  BytesValue result;
  try {
    result = service->Invoke(request.method.name, request.input);
  } catch (rpc::BadRequest& e) {
    LOG_ERROR("Bad RPC request {} (id = {}): {}", request.method,
                    request.id, e.what());
    RespondWithError(request, back, RPCErrorCode::BadRequest);
    return;
  } catch (...) {
    LOG_ERROR("Exception in {} (id = {}): {}", request.method, request.id,
                    wheels::CurrentExceptionMessage());
    RespondWithError(request, back, RPCErrorCode::ExecutionError);
    return;
  }

  // Ok
  SendResponse({request.id, request.method, result, RPCErrorCode::Ok}, back);
}

void ServerImpl::RespondWithError(const proto::Request& request,
                                  const ITransportSocketPtr& back,
                                  RPCErrorCode error) {
  SendResponse({request.id, request.method, "", error}, back);
}

void ServerImpl::SendResponse(proto::Response response,
                              const ITransportSocketPtr& back) {
  back->Send(Serialize(response));
}

}  // namespace whirl::rpc

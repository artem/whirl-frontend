#include <whirl/rpc/impl/transport_channel.hpp>

#include <await/futures/helpers.hpp>
#include <await/futures/await.hpp>

namespace whirl::rpc {

Future<BytesValue> RPCTransportChannel::Call(const Callee& callee,
                                             const BytesValue& input) {
  auto request = MakeRequest(callee, input);
  auto trace_id = request.trace_id;

  auto future = request.promise.MakeFuture();

  strand_->Execute(
      [self = shared_from_this(), request = std::move(request)]() mutable {
        self->SendRequest(std::move(request));
      });

  auto e = MakeTracingExecutor(executor_, trace_id);
  return std::move(future).Via(std::move(e));
}

void RPCTransportChannel::Close() {
  auto close = [self = shared_from_this()]() { self->DoClose(); };
  await::futures::SyncVia(strand_, std::move(close));
}

RPCTransportChannel::Request RPCTransportChannel::MakeRequest(
    const Callee& callee, const BytesValue& input) {
  Request request;

  request.id = GenerateRequestId();
  request.trace_id = GetOrGenerateNewTraceId(request.id);
  request.callee = callee;
  request.input = input;

  return request;
}

void RPCTransportChannel::SendRequest(Request request) {
  TLTraceContext tg{request.trace_id};

  WHIRL_FMT_LOG("Request method '{}' on peer {}", request.callee.ToString(),
                peer_);

  ITransportSocketPtr& socket = GetTransportSocket();

  if (!socket->IsConnected()) {
    // Fail RPC immediately
    Fail(request, make_error_code(RPCErrorCode::TransportError));
    return;
  }

  auto id = request.id;

  socket->Send(Serialize<RPCRequestMessage>(
      {request.id, request.trace_id, peer_, request.callee, request.input}));

  requests_.emplace(id, std::move(request));
}

void RPCTransportChannel::ProcessResponse(const TransportMessage& message) {
  WHIRL_FMT_LOG("Process response message from {}", peer_);

  auto response = ParseResponse(message);

  auto request_it = requests_.find(response.request_id);

  if (request_it == requests_.end()) {
    WHIRL_FMT_LOG("Request with id {} not found", response.request_id);
    return;  // Probably duplicated response message from transport layer?
  }

  Request request = std::move(request_it->second);
  requests_.erase(request_it);

  TLTraceContext tg{request.trace_id};

  if (response.IsOk()) {
    WHIRL_FMT_LOG("Request with id {} completed", response.request_id);
    std::move(request.promise).SetValue(response.result);
  } else {
    // TODO: better error
    Fail(request, make_error_code(response.error));
  }
}

RPCResponseMessage RPCTransportChannel::ParseResponse(
    const TransportMessage& message) {
  return Deserialize<RPCResponseMessage>(message);
}

void RPCTransportChannel::LostPeer() {
  auto requests = std::move(requests_);
  requests_.clear();

  WHIRL_FMT_LOG(
      "Transport connection to peer {} lost, fail {} pending request(s)", peer_,
      requests.size());

  // Next Call triggers reconnect
  socket_.reset();

  for (auto& [_, request] : requests) {
    TLTraceContext tg{request.trace_id};
    Fail(request, make_error_code(RPCErrorCode::TransportError));
  }
}

void RPCTransportChannel::DoClose() {
  if (socket_ && socket_->IsConnected()) {
    socket_->Close();
  }
}

ITransportSocketPtr& RPCTransportChannel::GetTransportSocket() {
  if (socket_) {
    return socket_;
  }
  WHIRL_FMT_LOG("Reconnect to {}", peer_);
  socket_ = transport_->ConnectTo(peer_, shared_from_this());
  return socket_;
}

void RPCTransportChannel::Fail(Request& request, std::error_code e) {
  WHIRL_FMT_LOG("Fail request with id = {}", request.id);
  std::move(request.promise).SetError(wheels::Error(e));
}

}  // namespace whirl::rpc

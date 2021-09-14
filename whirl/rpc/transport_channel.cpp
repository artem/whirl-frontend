#include <whirl/rpc/transport_channel.hpp>

#include <whirl/cereal/serialize.hpp>

#include <await/futures/helpers.hpp>
#include <await/futures/util/await.hpp>

#include <timber/log.hpp>

using await::futures::Future;

namespace whirl::rpc {

TransportChannel::~TransportChannel() {
  // NB: strand is empty (each task in strand holds strong reference to channel!
  // Close();
}

Future<BytesValue> TransportChannel::Call(const Method& method,
                                          const BytesValue& input,
                                          CallOptions options) {
  auto request = MakeRequest(method, input, options);
  auto trace_id = request.trace_id;

  auto future = request.promise.MakeFuture();

  await::executors::Execute(strand_,
      [self = shared_from_this(), request = std::move(request)]() mutable {
        self->SendRequest(std::move(request));
      });

  return std::move(future).Via(executor_);
}

void TransportChannel::Close() {
  // Do we need strong ref?
  auto close = [self = shared_from_this()]() {
    self->DoClose();
  };
  await::futures::SyncVia(&strand_, std::move(close));
}

TransportChannel::ActiveRequest TransportChannel::MakeRequest(
    const Method& method, const BytesValue& input, const CallOptions& options) {
  ActiveRequest request;

  request.id = GenerateRequestId();
  request.trace_id = options.trace_id;
  request.method = method;
  request.input = input;

  return request;
}

void TransportChannel::SendRequest(ActiveRequest request) {
  TLTraceContext tg{request.trace_id};

  LOG_INFO("Request {}.{}", peer_, request.method);

  ITransportSocketPtr& socket = GetTransportSocket();

  if (!socket->IsConnected()) {
    // Fail RPC immediately
    Fail(request, make_error_code(RPCErrorCode::TransportError));
    return;
  }

  auto id = request.id;

  socket->Send(Serialize<proto::Request>(
      {request.id, request.trace_id, peer_, request.method, request.input}));

  requests_.emplace(id, std::move(request));
}

void TransportChannel::ProcessResponse(const TransportMessage& message) {
  LOG_DEBUG("Process response message from {}", peer_);

  auto response = ParseResponse(message);

  auto request_it = requests_.find(response.request_id);

  if (request_it == requests_.end()) {
    LOG_WARN("Request with id {} not found", response.request_id);
    return;  // Probably duplicated response message from transport layer?
  }

  ActiveRequest request = std::move(request_it->second);
  requests_.erase(request_it);

  TLTraceContext tg{request.trace_id};

  if (response.IsOk()) {
    LOG_INFO("Request {}.{} with id = {} completed", peer_,
                   request.method, response.request_id);
    std::move(request.promise).SetValue(response.result);
  } else {
    // TODO: better error
    Fail(request, make_error_code(response.error));
  }
}

proto::Response TransportChannel::ParseResponse(
    const TransportMessage& message) {
  return Deserialize<proto::Response>(message);
}

void TransportChannel::LostPeer() {
  auto requests = std::move(requests_);
  requests_.clear();

  LOG_WARN(
      "Transport connection to peer {} lost, fail {} pending request(s)", peer_,
      requests.size());

  // Next Call triggers reconnect
  socket_.reset();

  for (auto& [_, request] : requests) {
    TLTraceContext tg{request.trace_id};
    Fail(request, make_error_code(RPCErrorCode::TransportError));
  }
}

void TransportChannel::DoClose() {
  LOG_INFO("Close transport socket");
  if (socket_ && socket_->IsConnected()) {
    socket_->Close();
  }
}

ITransportSocketPtr& TransportChannel::GetTransportSocket() {
  if (socket_ && socket_->IsConnected()) {
    return socket_;
  }
  LOG_DEBUG("Reconnect to {}", peer_);
  socket_ = transport_->ConnectTo(peer_, weak_from_this());
  return socket_;
}

void TransportChannel::Fail(ActiveRequest& request, std::error_code e) {
  LOG_WARN("Request {}.{} (id = {}) failed: {}", peer_, request.method,
                 request.id, e.message());
  std::move(request.promise).SetError(wheels::Error(e));
}

}  // namespace whirl::rpc

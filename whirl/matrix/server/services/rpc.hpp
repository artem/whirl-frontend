#pragma once

#include <whirl/services/rpc_impl.hpp>

#include <whirl/matrix/network/message.hpp>
#include <whirl/matrix/process/network.hpp>
#include <whirl/services/threads.hpp>
#include <whirl/matrix/process/heap.hpp>

#include <whirl/matrix/log/log.hpp>

#include <whirl/helpers/id.hpp>
#include <whirl/helpers/serialize.hpp>

#include <await/futures/promise.hpp>

#include <wheels/support/exception.hpp>

#include <cstdlib>
#include <map>
#include <string>
#include <functional>

#include <cereal/types/string.hpp>

namespace whirl {

using await::futures::Promise;

//////////////////////////////////////////////////////////////////////

using RPCId = Id;

//////////////////////////////////////////////////////////////////////

struct RPCError {
  int code;
  std::string message;

  bool IsOk() const {
    return code == 0;
  }

  static RPCError Ok() {
    return {0, ""};
  }

  SERIALIZE(CEREAL_NVP(code), CEREAL_NVP(message))
};

//////////////////////////////////////////////////////////////////////

// Network protocol

struct RPCRequestMessage {
  RPCId id;
  std::string server;  // For debugging
  std::string method;
  std::string input;

  SERIALIZE(CEREAL_NVP(id), CEREAL_NVP(server), CEREAL_NVP(method),
            CEREAL_NVP(input))
};

struct RPCResponseMessage {
  RPCId request_id;
  std::string method;  // For debugging
  std::string result;
  RPCError error;

  SERIALIZE(CEREAL_NVP(request_id), CEREAL_NVP(method), CEREAL_NVP(result),
            CEREAL_NVP(error))

  bool IsOk() const {
    return error.IsOk();
  }
};

//////////////////////////////////////////////////////////////////////

// Channel

class RPCChannel : public IRPCChannelImpl, public INetSocketHandler {
  using RequestPromise = Promise<std::string>;

  struct Request {
    RPCId id;
    RequestPromise promise;
  };

 public:
  RPCChannel(ProcessHeap& heap, ProcessNetwork& net, ServerAddress peer)
      : heap_(heap),
        net_(net),
        peer_(peer),
        socket_(net_.ConnectTo(peer_, this)) {
  }

  ~RPCChannel() {
    Shutdown();
  }

  // IRPCChannelImpl

  // Context: Server
  Future<RPCBytes> Call(const std::string& method,
                        const RPCBytes& input) override {
    WHIRL_LOG("Request method '" << method << "' on peer " << peer_);

    ProcessSocket& transport = GetTransportSocket();

    if (!transport.IsValid()) {
      // Fail RPC immediately
      return await::futures::MakeError(
          std::make_error_code(std::errc::connection_refused));
    }

    Request request;
    request.id = GenerateRequestId();
    auto future = request.promise.MakeFuture();
    requests_.emplace(request.id, std::move(request));
    transport.Send(
        Serialize<RPCRequestMessage>({request.id, peer_, method, input}));
    return future;
  }

  // No socket callbacks after shutdown
  void Shutdown() override {
    if (socket_.IsValid()) {
      socket_.Close();
    }
  }

  const std::string& Peer() const override {
    return peer_;
  }

  // INetSocketHandler

  // Context: global
  void HandleMessage(const Message& message,
                     LightNetSocket /*socket*/) override {
    auto g = heap_.Use();

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

  // Context: global
  void HandlePeerLost() override {
    auto g = heap_.Use();

    WHIRL_LOG("Socket lost, fail all pending requests");
    // Fail all pending requests
    for (auto& [id, request] : requests_) {
      Fail(request, std::errc::connection_reset);
    }
    requests_.clear();
    // TODO: close socket!

    socket_.Close();
  }

 private:
  ProcessSocket& GetTransportSocket() {
    if (socket_.IsValid()) {
      return socket_;
    }
    WHIRL_LOG("Invalid socket, reconnect to " << peer_);
    socket_ = net_.ConnectTo(peer_, this);
    return socket_;
  }

  void Fail(Request& request, std::errc e) {
    std::move(request.promise).SetError(std::make_error_code(e));
  }

  static RPCId GenerateRequestId() {
    static IdGenerator ids_;
    return ids_.NextId();
  }

 private:
  ProcessHeap& heap_;
  ProcessNetwork& net_;
  ServerAddress peer_;

  std::map<RPCId, Request> requests_;

  ProcessSocket socket_;

  Logger logger_{"RPC channel"};
};

//////////////////////////////////////////////////////////////////////

class RPCClient : public IRPCClientImpl {
 public:
  RPCClient(ProcessHeap& heap, ProcessNetwork& net) : heap_(heap), net_(net) {
  }

  IRPCChannelImplPtr Dial(const ServerAddress& peer) {
    return std::make_shared<RPCChannel>(heap_, net_, peer);
  }

 private:
  ProcessHeap& heap_;
  ProcessNetwork& net_;
};

//////////////////////////////////////////////////////////////////////

// Server

class RPCServer : public INetSocketHandler, public IRPCServerImpl {
 public:
  RPCServer(ProcessHeap& heap, ProcessNetwork& net, IExecutorPtr executor)
      : heap_(heap), net_(net), executor_(executor), socket_(net_.Serve(this)) {
  }

  // IRPCServer

  // Context: Server/Node
  void RegisterMethod(const std::string& method,
                      RPCMethodInvoker invoker) override {
    if (methods_.find(method) != methods_.end()) {
      WHEELS_PANIC("RPC method '" << method << "' already registered");
    }

    methods_.emplace(method, std::move(invoker));
  }

  // INetSocketHandler

  // Context: global
  void HandleMessage(const Message& message, LightNetSocket back) override {
    auto g = heap_.Use();

    // Process request
    await::fibers::Spawn(
        [this, message, back = std::move(back)]() mutable {
          ProcessRequest(message, std::move(back));
        },
        executor_);
  }

  // Context: global
  void HandlePeerLost() override {
    auto g = heap_.Use();
    // Some client lost
  }

 private:
  std::string MakeRequestThreadName(const RPCRequestMessage& request) {
    return wheels::StringBuilder() << "rpc-" << request.id;
  }

  void ProcessRequest(const Message& message, LightNetSocket back) {
    auto request = Deserialize<RPCRequestMessage>(message);

    //await::fibers::SetName(MakeRequestThreadName(request));

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

  void ResponseWithError(const RPCRequestMessage& request, LightNetSocket& back,
                         RPCError error) {
    SendResponse({request.id, request.method, "", error}, back);
  }

  void SendResponse(RPCResponseMessage response, LightNetSocket& back) {
    back.Send(Serialize(response));
  }

 private:
  ProcessHeap& heap_;
  ProcessNetwork& net_;
  IExecutorPtr executor_;
  ProcessServerSocket socket_;

  std::map<std::string, RPCMethodInvoker> methods_;

  Logger logger_{"RPC server"};
};

}  // namespace whirl

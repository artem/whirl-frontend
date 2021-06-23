#pragma once

#include <whirl/rpc/bytes_value.hpp>
#include <whirl/rpc/protocol.hpp>
#include <whirl/rpc/net_transport.hpp>
#include <whirl/rpc/server.hpp>

#include <await/executors/executor.hpp>

// TODO: abstract logger
#include <whirl/engines/matrix/log/logger.hpp>

#include <functional>
#include <memory>
#include <map>

namespace whirl::rpc {

using await::executors::IExecutorPtr;

// Server

//////////////////////////////////////////////////////////////////////

class ServerImpl : public IServer,
                   public std::enable_shared_from_this<ServerImpl>,
                   public ITransportHandler {
 public:
  ServerImpl(ITransportPtr t, IExecutorPtr e) : transport_(t), executor_(e) {
  }

  void Start() override;
  void RegisterService(const std::string& name, IServicePtr service) override;
  void Shutdown() override;

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr back) override;

  void HandleDisconnect(const std::string& client) override;

 private:
  // In separate fiber
  void ProcessRequest(const TransportMessage& message,
                      const ITransportSocketPtr& back);

  void RespondWithError(const RequestMessage& request,
                        const ITransportSocketPtr& back, RPCErrorCode error);

  void SendResponse(ResponseMessage response, const ITransportSocketPtr& back);

 private:
  // Services
  ITransportPtr transport_;
  IExecutorPtr executor_;

  ITransportServerPtr server_;

  // Names -> Services
  std::map<std::string, IServicePtr> services_;

  Logger logger_{"RPC-Server"};
};

}  // namespace whirl::rpc

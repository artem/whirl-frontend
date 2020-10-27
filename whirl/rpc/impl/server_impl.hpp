#pragma once

#include <whirl/rpc/impl/bytes_value.hpp>
#include <whirl/rpc/impl/protocol.hpp>
#include <whirl/rpc/impl/net_transport.hpp>
#include <whirl/rpc/impl/server.hpp>

#include <await/executors/executor.hpp>

// TODO: abstract logger
#include <whirl/matrix/log/logger.hpp>

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

  void Start(std::string port) override;
  void RegisterService(const std::string& name, IServicePtr service) override;
  void Shutdown() override;

  // ITransportHandler

  void HandleMessage(const TransportMessage& message,
                     ITransportSocketPtr back) override;

  void HandleDisconnect() override;

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

  std::map<std::string, IServicePtr> services_;

  Logger logger_{"RPC server"};
};

}  // namespace whirl::rpc

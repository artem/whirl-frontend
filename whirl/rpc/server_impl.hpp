#pragma once

#include <whirl/services/net_transport.hpp>

#include <whirl/rpc/bytes_value.hpp>
#include <whirl/rpc/protocol.hpp>
#include <whirl/rpc/server.hpp>

#include <whirl/logger/log.hpp>

#include <await/executors/executor.hpp>
#include <await/fibers/core/manager.hpp>

#include <functional>
#include <memory>
#include <map>

namespace whirl::rpc {

// Server

//////////////////////////////////////////////////////////////////////

class ServerImpl : public IServer,
                   public std::enable_shared_from_this<ServerImpl>,
                   public ITransportHandler {
 public:
  ServerImpl(ITransportPtr t,
             await::executors::IExecutorPtr e,
             await::fibers::IFiberManager* fm)
      : transport_(t),
        executor_(e),
        fiber_manager_(fm) {
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

  void RespondWithError(const proto::Request& request,
                        const ITransportSocketPtr& back, RPCErrorCode error);

  void SendResponse(proto::Response response, const ITransportSocketPtr& back);

 private:
  // Services
  ITransportPtr transport_;
  await::executors::IExecutorPtr executor_;
  await::fibers::IFiberManager* fiber_manager_;

  ITransportServerPtr server_;

  // Names -> Services
  std::map<std::string, IServicePtr> services_;

  Logger logger_{"RPC-Server"};
};

}  // namespace whirl::rpc

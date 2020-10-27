#pragma once

#include <whirl/rpc/impl/service.hpp>

namespace whirl::rpc {

struct IServer {
  virtual ~IServer() = default;

  virtual void Start(std::string port) = 0;
  virtual void RegisterService(const std::string& name,
                               IServicePtr service) = 0;
  virtual void Shutdown() = 0;
};

using IServerPtr = std::shared_ptr<IServer>;

}  // namespace whirl::rpc

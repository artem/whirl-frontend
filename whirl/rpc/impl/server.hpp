#pragma once

#include <whirl/rpc/impl/service.hpp>

namespace whirl::rpc {

struct IRPCServer {
  virtual ~IRPCServer() = default;

  virtual void Start() = 0;
  virtual void RegisterService(const std::string& name,
                               IRPCServicePtr service) = 0;
  virtual void Shutdown() = 0;
};

using IRPCServerPtr = std::shared_ptr<IRPCServer>;

}  // namespace whirl::rpc

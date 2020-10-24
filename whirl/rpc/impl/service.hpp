#pragma once

#include <whirl/rpc/impl/raw_value.hpp>

namespace whirl::rpc {

struct IRPCService {
  virtual ~IRPCService() = default;

  virtual void Initialize() = 0;

  virtual bool Has(const std::string& method) const = 0;
  virtual BytesValue Invoke(const std::string& method,
                            const BytesValue& input) = 0;
};

using IRPCServicePtr = std::shared_ptr<IRPCService>;

}  // namespace whirl::rpc

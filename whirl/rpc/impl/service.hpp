#pragma once

#include <whirl/rpc/impl/raw_value.hpp>

namespace whirl::rpc {

struct IService {
  virtual ~IService() = default;

  virtual void Initialize() = 0;

  virtual bool Has(const std::string& method) const = 0;
  virtual BytesValue Invoke(const std::string& method,
                            const BytesValue& input) = 0;
};

using IServicePtr = std::shared_ptr<IService>;

}  // namespace whirl::rpc

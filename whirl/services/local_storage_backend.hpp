#pragma once

#include <memory>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using Bytes = std::string;

//////////////////////////////////////////////////////////////////////

struct ILocalStorageBackend {
  virtual ~ILocalStorageBackend() = default;

  virtual void Set(const std::string& key, const Bytes& value) = 0;
  virtual bool Has(const std::string& key) const = 0;
  virtual Bytes Get(const std::string& key) = 0;
};

using ILocalStorageBackendPtr = std::shared_ptr<ILocalStorageBackend>;

}  // namespace whirl

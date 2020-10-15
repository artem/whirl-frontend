#pragma once

#include <memory>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using Bytes = std::string;

//////////////////////////////////////////////////////////////////////

struct ILocalStorageBackend {
  virtual ~ILocalStorageBackend() = default;

  virtual void Set(const Bytes& key, const Bytes& value) = 0;
  virtual bool Has(const Bytes& key) const = 0;
  virtual Bytes Get(const Bytes& key) = 0;
};

using ILocalStorageBackendPtr = std::shared_ptr<ILocalStorageBackend>;

}  // namespace whirl

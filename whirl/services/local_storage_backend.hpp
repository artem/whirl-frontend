#pragma once

#include <memory>
#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using Bytes = std::string;

//////////////////////////////////////////////////////////////////////

struct ILocalStorageBackend {
  virtual ~ILocalStorageBackend() = default;

  virtual void Set(const std::string& key, const Bytes& value) = 0;
  virtual std::optional<Bytes> TryGet(const std::string& key) = 0;
  virtual void Remove(const std::string& key) = 0;
};

using ILocalStorageBackendPtr = std::shared_ptr<ILocalStorageBackend>;

}  // namespace whirl

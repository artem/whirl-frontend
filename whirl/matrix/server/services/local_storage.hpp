#pragma once

#include <whirl/services/local_storage_backend.hpp>

#include <whirl/matrix/server/storage.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LocalStorageBackend : public ILocalStorageBackend {
 public:
  LocalStorageBackend(LocalBytesStorage& impl) : impl_(impl) {
  }

  void Set(const std::string& key, const Bytes& value) override {
    impl_.Set(key, value);
  }

  std::optional<Bytes> TryGet(const std::string& key) override {
    return impl_.TryGet(key);
  }

 private:
  LocalBytesStorage& impl_;
};

}  // namespace whirl

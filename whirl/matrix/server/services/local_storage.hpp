#pragma once

#include <whirl/services/local_storage_backend.hpp>

#include <whirl/matrix/server/local_storage.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LocalStorageBackend : public ILocalStorageBackend {
 public:
  LocalStorageBackend(LocalBytesStorage& impl) : impl_(impl) {
  }

  Bytes Get(const std::string& key) {
    return impl_.Get(key);
  }

  bool Has(const std::string& key) const {
    return impl_.Has(key);
  }

  void Set(const std::string& key, const Bytes& value) {
    impl_.Set(key, value);
  }

 private:
  LocalBytesStorage& impl_;
};

}  // namespace whirl

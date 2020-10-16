#pragma once

#include <whirl/services/local_storage_backend.hpp>

#include <whirl/matrix/common/copy.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Local storage service impl

class LocalBytesStorage {
 public:
  Bytes Get(const std::string& key) {
    auto found = data_.find(key);
    if (found != data_.end()) {
      return MakeCopy(found->second);  // string allocated in node's heap
    } else {
      return {};
    }
  }

  bool Has(const std::string& key) const {
    GlobalHeapScope g;
    return data_.count(key) != 0;
  }

  void Set(const std::string& key, const Bytes& value) {
    GlobalHeapScope g;

    auto found = data_.find(key);
    if (found != data_.end()) {
      found->second = value;  // Copy
    } else {
      data_.insert(std::make_pair(key, value));  // Copy
    }
  }

 private:
  std::map<Bytes, Bytes> data_;
};

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

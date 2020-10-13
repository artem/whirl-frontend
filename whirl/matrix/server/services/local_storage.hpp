#pragma once

#include <whirl/services/local_storage_impl.hpp>

#include <whirl/matrix/common/copy.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Local storage service impl

class LocalBytesStorage {
 public:
  Bytes Get(const Bytes& key) {
    auto found = data_.find(key);
    if (found != data_.end()) {
      return MakeCopy(found->second);  // string allocated in node's heap
    } else {
      return {};
    }
  }

  bool Has(const Bytes& key) const {
    GlobalHeapScope g;
    return data_.count(key) != 0;
  }

  void Set(const Bytes& key, const Bytes& value) {
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

class LocalStorageEngine : public ILocalStorageEngine {
 public:
  LocalStorageEngine(LocalBytesStorage& impl) : impl_(impl) {
  }

  Bytes Get(const Bytes& key) {
    return impl_.Get(key);
  }

  bool Has(const Bytes& key) const {
    return impl_.Has(key);
  }

  void Set(const Bytes& key, const Bytes& value) {
    impl_.Set(key, value);
  }

 private:
  LocalBytesStorage& impl_;
};

}  // namespace whirl

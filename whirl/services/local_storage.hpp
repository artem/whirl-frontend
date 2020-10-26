#pragma once

#include <whirl/services/local_storage_backend.hpp>

#include <whirl/helpers/serialize.hpp>

#include <optional>

namespace whirl {

// Typed local storages

//////////////////////////////////////////////////////////////////////

// Persistent mapping: string -> V

template <typename V>
class LocalKVStorage {
 public:
  LocalKVStorage(ILocalStorageBackendPtr kv, const std::string& name)
      : impl_(kv), namespace_(MakeNamespace(name)) {
  }

  // Non-copyable
  LocalKVStorage(const LocalKVStorage& that) = delete;
  LocalKVStorage& operator=(const LocalKVStorage& that) = delete;

  void Set(const std::string& key, const V& value) {
    auto value_bytes = Serialize(value);
    impl_->Set(WithNamespace(key), value_bytes);
  }

  bool Has(const std::string& key) const {
    return impl_->Has(WithNamespace(key));
  }

  V Get(const std::string& key) {
    auto value_bytes = impl_->Get(WithNamespace(key));
    return Deserialize<V>(value_bytes);
  }

  std::optional<V> TryGet(const std::string& key) {
    if (Has(key)) {
      return Get(key);
    } else {
      return std::nullopt;
    }
  }

  V GetOr(const std::string& key, V default_value) {
    if (Has(key)) {
      return Get(key);
    } else {
      return default_value;
    }
  }

 private:
  static std::string MakeNamespace(const std::string& name) {
    return std::string("kv:") + name + ":";
  }

  std::string WithNamespace(const std::string& user_key) const {
    return namespace_ + user_key;
  }

 private:
  ILocalStorageBackendPtr impl_;
  std::string namespace_;
};

//////////////////////////////////////////////////////////////////////

// Persistent map from string to arbitrary values

// Usage:
// * local_storage.Store("epoch", 42)
// * local_storage_.Load<int>("epoch");

class LocalStorage {
 public:
  LocalStorage(ILocalStorageBackendPtr impl) : impl_(impl) {
  }

  // Non-copyable
  LocalStorage(const LocalStorage& that) = delete;
  LocalStorage& operator=(const LocalStorage& that) = delete;

  bool Has(const std::string& key) const {
    return impl_->Has(WithNamespace(key));
  }

  template <typename U>
  void Store(const std::string& key, const U& data) {
    auto data_bytes = Serialize(data);
    impl_->Set(WithNamespace(key), data_bytes);
  }

  template <typename U>
  U Load(const std::string& key) {
    auto data_bytes = impl_->Get(WithNamespace(key));
    return Deserialize<U>(data_bytes);
  }

 private:
  std::string WithNamespace(const std::string& user_key) const {
    static const std::string kNamespace = "local:";
    return kNamespace + user_key;
  }

 private:
  ILocalStorageBackendPtr impl_;
};

}  // namespace whirl

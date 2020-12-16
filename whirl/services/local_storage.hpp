#pragma once

#include <whirl/services/local_storage_backend.hpp>

#include <whirl/helpers/bytes.hpp>

#include <fmt/core.h>

#include <optional>
#include <stdexcept>

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
    auto value_bytes = Bytes::Serialize(value);
    impl_->Set(WithNamespace(key), value_bytes);
  }

  bool Has(const std::string& key) const {
    std::optional<Bytes> value_bytes = impl_->TryGet(WithNamespace(key));
    return value_bytes.has_value();
  }

  std::optional<V> TryGet(const std::string& key) const {
    std::optional<Bytes> value_bytes = impl_->TryGet(WithNamespace(key));
    if (value_bytes.has_value()) {
      return value_bytes->As<V>();
    } else {
      return std::nullopt;
    }
  }

  V Get(const std::string& key) const {
    std::optional<V> existing_value = TryGet(key);
    if (existing_value.has_value()) {
      return *existing_value;
    } else {
      throw std::runtime_error(
          fmt::format("Key '{}' not found in local KV storage", WithNamespace(key)));
    }
  }

  V GetOr(const std::string& key, V default_value) const {
    std::optional<V> existing_value = TryGet(key);
    if (existing_value.has_value()) {
      return *existing_value;
    } else {
      return default_value;
    }
  }

  void Remove(const std::string& key) {
    impl_->Remove(WithNamespace(key));
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
// * local_storage.Store<int>("epoch", 42)
// * local_storage.Load<int>("epoch");

class LocalStorage {
 public:
  LocalStorage(ILocalStorageBackendPtr impl) : impl_(impl) {
  }

  // Non-copyable
  LocalStorage(const LocalStorage& that) = delete;
  LocalStorage& operator=(const LocalStorage& that) = delete;

  template <typename U>
  void Store(const std::string& key, const U& data) {
    auto data_bytes = Bytes::Serialize(data);
    impl_->Set(WithNamespace(key), data_bytes);
  }

  bool Has(const std::string& key) const {
    std::optional<Bytes> data_bytes = impl_->TryGet(WithNamespace(key));
    return data_bytes.has_value();
  }

  template <typename U>
  std::optional<U> TryLoad(const std::string& key) const {
    std::optional<Bytes> data_bytes = impl_->TryGet(WithNamespace(key));
    if (data_bytes.has_value()) {
      return data_bytes->As<U>();
    } else {
      return std::nullopt;
    }
  }

  template <typename U>
  U Load(const std::string& key) const {
    std::optional<U> value = TryLoad<U>(key);
    if (value.has_value()) {
      return *value;
    } else {
      throw std::runtime_error(fmt::format("Key '{}' not found in local storage", WithNamespace(key)));
    }
  }

  void Remove(const std::string& key) {
    impl_->Remove(WithNamespace(key));
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

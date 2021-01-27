#pragma once

#include <whirl/services/database.hpp>

#include <whirl/cereal/bytes.hpp>

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
  LocalKVStorage(IDatabasePtr db, const std::string& name)
      : db_(db), namespace_(MakeNamespace(name)) {
  }

  // Non-copyable
  LocalKVStorage(const LocalKVStorage& that) = delete;
  LocalKVStorage& operator=(const LocalKVStorage& that) = delete;

  void Set(const std::string& key, const V& value) {
    auto value_bytes = Bytes::Serialize(value);
    db_->Put(WithNamespace(key), value_bytes);
  }

  bool Has(const std::string& key) const {
    std::optional<Bytes> value_bytes = db_->TryGet(WithNamespace(key));
    return value_bytes.has_value();
  }

  std::optional<V> TryGet(const std::string& key) const {
    std::optional<Bytes> value_bytes = db_->TryGet(WithNamespace(key));
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
      throw std::runtime_error(fmt::format(
          "Key '{}' not found in local KV storage", WithNamespace(key)));
    }
  }

  V GetOr(const std::string& key, V default_value) const {
    return TryGet(key).value_or(default_value);
  }

  void Delete(const std::string& key) {
    db_->Delete(WithNamespace(key));
  }

 private:
  static std::string MakeNamespace(const std::string& name) {
    return fmt::format("kv:{}:", name);
  }

  std::string WithNamespace(const std::string& user_key) const {
    return namespace_ + user_key;
  }

 private:
  IDatabasePtr db_;
  std::string namespace_;
};

//////////////////////////////////////////////////////////////////////

// Persistent map from string to arbitrary values

// Usage:
// * local_storage.Store<int>("epoch", 42)
// * local_storage.Load<int>("epoch");

class LocalStorage {
 public:
  LocalStorage(IDatabasePtr db, const std::string& name = "default")
      : db_(db), namespace_(MakeNamespace(name)) {
  }

  // Non-copyable
  LocalStorage(const LocalStorage& that) = delete;
  LocalStorage& operator=(const LocalStorage& that) = delete;

  template <typename U>
  void Store(const std::string& key, const U& data) {
    auto data_bytes = Bytes::Serialize(data);
    db_->Put(WithNamespace(key), data_bytes);
  }

  bool Has(const std::string& key) const {
    std::optional<Bytes> data_bytes = db_->TryGet(WithNamespace(key));
    return data_bytes.has_value();
  }

  template <typename U>
  std::optional<U> TryLoad(const std::string& key) const {
    std::optional<Bytes> data_bytes = db_->TryGet(WithNamespace(key));
    if (data_bytes.has_value()) {
      return data_bytes->As<U>();
    } else {
      return std::nullopt;
    }
  }

  template <typename U>
  U LoadOr(const std::string& key, U default_value) const {
    return TryLoad<U>(key).value_or(default_value);
  }

  template <typename U>
  U Load(const std::string& key) const {
    std::optional<U> value = TryLoad<U>(key);
    if (value.has_value()) {
      return *value;
    } else {
      throw std::runtime_error(fmt::format(
          "Key '{}' not found in local storage", WithNamespace(key)));
    }
  }

  void Delete(const std::string& key) {
    db_->Delete(WithNamespace(key));
  }

 private:
  static std::string MakeNamespace(const std::string& name) {
    return fmt::format("local:{}:", name);
  }

  std::string WithNamespace(const std::string& user_key) const {
    return namespace_ + user_key;
  }

 private:
  IDatabasePtr db_;
  std::string namespace_;
};

}  // namespace whirl

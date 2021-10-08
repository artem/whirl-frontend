#pragma once

#include <whirl/node/db/database.hpp>

#include <muesli/bytes.hpp>

#include <fmt/core.h>

#include <stdexcept>

namespace whirl::node::store {

// Persistent map from string to arbitrary values

// Usage:
// * store.Store<int>("epoch", 42)
// * store.Load<int>("epoch");

class Store {
 public:
  Store(db::IDatabase* db, const std::string& name = "default")
      : db_(db), namespace_(MakeNamespace(name)) {
  }

  // Non-copyable
  Store(const Store&) = delete;
  Store& operator=(const Store&) = delete;

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
  U LoadOr(const std::string& key, U or_value) const {
    return TryLoad<U>(key).value_or(or_value);
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
    return fmt::format("struct:{}:", name);
  }

  std::string WithNamespace(const std::string& user_key) const {
    return namespace_ + user_key;
  }

 private:
  db::IDatabase* db_;
  std::string namespace_;
};

}  // namespace whirl::node::store

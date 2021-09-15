#pragma once

#include <whirl/node/db/database.hpp>

#include <whirl/cereal/bytes.hpp>

#include <fmt/core.h>

#include <stdexcept>

namespace whirl::node::store {

// Persistent mapping: string -> V

template <typename V>
class KVStore {
 public:
  KVStore(db::IDatabase* db, const std::string& name)
      : db_(db), namespace_(MakeNamespace(name)) {
  }

  // Non-copyable
  KVStore(const KVStore&) = delete;
  KVStore& operator=(const KVStore&) = delete;

  void Put(const std::string& key, const V& value) {
    auto value_bytes = Serialize(value);
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

  V GetOr(const std::string& key, V or_value) const {
    return TryGet(key).value_or(or_value);
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
  db::IDatabase* db_;
  std::string namespace_;
};

}  // namespace whirl::node::store

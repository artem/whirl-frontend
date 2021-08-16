#pragma once

#include <whirl/db/kv.hpp>

#include <optional>

namespace whirl::node::db {

// ~ LevelDB (https://github.com/google/leveldb)
// Ordered mapping from string keys to string values.

// TODO: Iterators

struct IDatabase {
  virtual ~IDatabase() = default;

  virtual void Open(const std::string& directory) = 0;

  // Atomic, blocking
  virtual void Put(const Key& key, const Value& value) = 0;
  virtual std::optional<Value> TryGet(const Key& key) const = 0;
  virtual void Delete(const Key& key) = 0;
};

}  // namespace whirl::node::db

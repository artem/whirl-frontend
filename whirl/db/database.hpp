#pragma once

#include <whirl/db/kv.hpp>

#include <optional>

namespace whirl {

// ~ LevelDB (https://github.com/google/leveldb)
// Ordered mapping from string keys to string values.

// TODO: Iterators

struct IDatabase {
  virtual ~IDatabase() = default;

  virtual void Open(const std::string& directory) = 0;

  // Atomic, blocking
  virtual void Put(const DbKey& key, const DbValue& value) = 0;
  virtual std::optional<DbValue> TryGet(const DbKey& key) const = 0;
  virtual void Delete(const DbKey& key) = 0;
};

}  // namespace whirl

#pragma once

#include <whirl/node/db/kv.hpp>
#include <whirl/node/db/write_batch.hpp>
#include <whirl/node/db/snapshot.hpp>

#include <optional>

namespace whirl::node::db {

// ~ LevelDB (https://github.com/google/leveldb)
// Ordered mapping from string keys to string values.

// TODO: Iterators

struct IDatabase {
  virtual ~IDatabase() = default;

  virtual void Open(const std::string& directory) = 0;

  // All operations are synchronous!

  // Single-key operations
  virtual void Put(const Key& key, const Value& value) = 0;
  virtual std::optional<Value> TryGet(const Key& key) const = 0;
  virtual void Delete(const Key& key) = 0;

  // Multi-key atomic write
  virtual void Write(WriteBatch batch) = 0;

  // Snapshots, iteration
  virtual ISnapshotPtr MakeSnapshot() = 0;
};

}  // namespace whirl::node::db

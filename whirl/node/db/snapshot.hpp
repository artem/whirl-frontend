#pragma once

#include <whirl/node/db/iterator.hpp>

#include <memory>
#include <optional>

namespace whirl::node::db {

// Immutable stable snapshot

struct ISnapshot {
  virtual ~ISnapshot() = default;

  virtual std::optional<Value> TryGet(const Key& key) const = 0;

  virtual IIteratorPtr MakeIterator() = 0;
};

using ISnapshotPtr = std::shared_ptr<ISnapshot>;

}  // namespace whirl::node::db

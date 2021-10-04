#pragma once

#include <whirl/node/db/iterator.hpp>

#include <memory>

namespace whirl::node::db {

struct ISnapshot {
  virtual ~ISnapshot() = default;

  virtual IIteratorPtr MakeIterator() = 0;
};

using ISnapshotPtr = std::shared_ptr<ISnapshot>;

}  // namespace whirl::node::db

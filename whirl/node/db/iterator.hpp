#pragma once

#include <whirl/node/db/kv.hpp>

#include <memory>

namespace whirl::node::db {

struct IIterator {
  virtual ~IIterator() = default;

  virtual bool Valid() const = 0;

  virtual db::Key Key() const = 0;
  virtual db::ValueView Value() const = 0;

  // Position at the first key that is at or past `target`
  virtual void Seek(db::Key target) = 0;

  virtual void SeekToLast() = 0;
  virtual void SeekToFirst() = 0;

  virtual void Next() = 0;
  virtual void Prev() = 0;
};

using IIteratorPtr = std::shared_ptr<IIterator>;

}  // namespace whirl::node::db

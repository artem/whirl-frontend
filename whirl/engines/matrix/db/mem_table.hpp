#pragma once

#include <whirl/db/kv.hpp>

#include <map>

namespace whirl::matrix::db {

// Sorted in-memory string -> string mapping

class MemTable {
 public:
  MemTable() = default;

  void Put(DbKey key, DbValue value) {
    entries_.insert_or_assign(key, value);
  }

  std::optional<DbValue> TryGet(DbKey key) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
      return it->second;
    } else {
      return std::nullopt;
    }
  }

  void Delete(DbKey key) {
    entries_.erase(key);
  }

  void Clear() {
    entries_.clear();
  }

 private:
  std::map<DbKey, DbValue> entries_;
};

}  // namespace whirl::matrix::db

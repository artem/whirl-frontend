#pragma once

#include <whirl/db/mutation.hpp>

#include <vector>

namespace whirl::node::db {

struct WriteBatch {
  std::vector<Mutation> muts_;

  void Put(Key key, Value value) {
    muts_.push_back({MutationType::Put, std::move(key), std::move(value)});
  }

  void Delete(Key key) {
    muts_.push_back({MutationType::Delete, std::move(key), std::nullopt});
  }
};

}  // namespace whirl::node::db

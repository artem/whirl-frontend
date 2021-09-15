#pragma once

#include <whirl/node/db/mutation.hpp>

#include <vector>

namespace whirl::node::db {

struct WriteBatch {
  std::vector<Mutation> muts;

  void Put(Key key, Value value) {
    muts.push_back({MutationType::Put, std::move(key), std::move(value)});
  }

  void Delete(Key key) {
    muts.push_back({MutationType::Delete, std::move(key), std::nullopt});
  }
};

}  // namespace whirl::node::db

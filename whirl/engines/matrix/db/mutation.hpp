#pragma once

#include <whirl/db/kv.hpp>

#include <whirl/cereal/serializable.hpp>

#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>

#include <cstdlib>

namespace whirl::matrix::db {

struct MutationType {
  using Value = int32_t;

  enum _ {
    Put = 0,  // Do not format
    Delete = 1
  };
};

struct Mutation {
  node::db::Key key;
  std::optional<node::db::Value> value;
  MutationType::Value type;

  WHIRL_SERIALIZE(key, value, type)
};

}  // namespace whirl::matrix::db

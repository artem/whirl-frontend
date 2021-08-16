#pragma once

#include <whirl/db/kv.hpp>

#include <whirl/cereal/serializable.hpp>

#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>

#include <cstdlib>

namespace whirl::node::db {

struct MutationType {
  using Value = int32_t;

  enum _ {
    Put = 0,  // Do not format
    Delete = 1
  };
};

struct Mutation {
  Key key;
  std::optional<Value> value;
  MutationType::Value type;

  WHIRL_SERIALIZE(key, value, type)
};

}  // namespace whirl::node::db

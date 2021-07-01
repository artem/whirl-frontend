#pragma once

#include <whirl/cereal/bytes.hpp>

#include <memory>
#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// ~ LevelDB (https://github.com/google/leveldb)
// Ordered mapping from string keys to string values.

// TODO: Iterators

struct IDatabase {
  virtual ~IDatabase() = default;

  virtual void Put(const std::string& key, const Bytes& value) = 0;
  virtual std::optional<Bytes> TryGet(const std::string& key) const = 0;
  virtual void Delete(const std::string& key) = 0;
};

using IDatabasePtr = IDatabase*;

}  // namespace whirl

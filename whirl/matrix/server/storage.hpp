#pragma once

#include <whirl/helpers/bytes.hpp>

#include <map>
#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class PersistentStorage {
 public:
  // Context: Server
  void Set(const std::string& key, const Bytes& value);
  std::optional<Bytes> TryGet(const std::string& key) const;
  void Remove(const std::string& key);

  // Context: Global
  size_t ComputeDigest() const;

 private:
  std::map<std::string, Bytes> data_;
};

}  // namespace whirl

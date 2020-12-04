#pragma once

#include <whirl/matrix/memory/allocator.hpp>
#include <whirl/matrix/memory/helpers.hpp>

#include <whirl/helpers/digest.hpp>

#include <map>
#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LocalBytesStorage {
  using Bytes = std::string;

 public:
  void Set(const std::string& key, const Bytes& value) {
    GlobalHeapScope g;

    auto found = data_.find(key);
    if (found != data_.end()) {
      found->second = value;  // Copy
    } else {
      data_.insert(std::make_pair(key, value));  // Copy
    }
  }

  // Context: Server
  std::optional<Bytes> TryGet(const std::string& key) {
    auto found = data_.find(key);
    if (found != data_.end()) {
      return MakeCopy(found->second);  // string allocated in node's heap
    } else {
      return std::nullopt;
    }
  }

  // Context: Global
  size_t ComputeDigest() const {
    DigestCalculator digest;
    // NB: sorted std::map, unique keys -> deterministic order
    for (const auto& [k, v] : data_) {
      digest.EatT(k).EatT(v);
    }
    return digest.Get();
  }

 private:
  std::map<std::string, Bytes> data_;
};

}  // namespace whirl

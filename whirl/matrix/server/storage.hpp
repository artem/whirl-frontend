#pragma once

#include <whirl/matrix/common/allocator.hpp>
#include <whirl/matrix/common/copy.hpp>

#include <whirl/helpers/digest.hpp>

#include <map>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LocalBytesStorage {
  using Bytes = std::string;

 public:
  Bytes Get(const std::string& key) {
    auto found = data_.find(key);
    if (found != data_.end()) {
      return MakeCopy(found->second);  // string allocated in node's heap
    } else {
      return {};
    }
  }

  bool Has(const std::string& key) const {
    GlobalHeapScope g;
    return data_.count(key) != 0;
  }

  void Set(const std::string& key, const Bytes& value) {
    GlobalHeapScope g;

    auto found = data_.find(key);
    if (found != data_.end()) {
      found->second = value;  // Copy
    } else {
      data_.insert(std::make_pair(key, value));  // Copy
    }
  }

  size_t ComputeDigest() const {
    DigestCalculator digest;
    for (const auto& [k, v] : data_) {
      digest.EatT(k).EatT(v);
    }
    return digest.Get();
  }

 private:
  std::map<std::string, Bytes> data_;
};

}  // namespace whirl

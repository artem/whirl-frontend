#include <whirl/matrix/server/storage.hpp>

#include <whirl/matrix/memory/allocator.hpp>
#include <whirl/matrix/memory/helpers.hpp>

#include <whirl/helpers/digest.hpp>

namespace whirl {

// Context: Server
void PersistentStorage::Set(const std::string& key, const Bytes& value) {
  GlobalHeapScope g;

  if (auto found = data_.find(key); found != data_.end()) {
    found->second = value;  // Copy
  } else {
    data_.insert(std::make_pair(key, value));  // Copy
  }
}

// Context: Server
std::optional<PersistentStorage::Bytes> PersistentStorage::TryGet(const std::string& key) {
  if (auto found = data_.find(key); found != data_.end()) {
    return MakeCopy(found->second);  // Copy
  } else {
    return std::nullopt;
  }
}

void PersistentStorage::Remove(const std::string& key) {
  GlobalHeapScope g;
  data_.erase(key);
}

// Context: Global
size_t PersistentStorage::ComputeDigest() const {
  DigestCalculator digest;
  // NB: sorted std::map, unique keys -> deterministic order
  for (const auto& [k, v] : data_) {
    digest.EatT(k).EatT(v);
  }
  return digest.Get();
}

}  // namespace whirl

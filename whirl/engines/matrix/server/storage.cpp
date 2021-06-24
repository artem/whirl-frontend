#include <whirl/engines/matrix/server/storage.hpp>

#include <whirl/engines/matrix/memory/new.hpp>
#include <whirl/engines/matrix/memory/helpers.hpp>

#include <whirl/helpers/digest.hpp>

namespace whirl {

// Context: Server
void PersistentStorage::Put(const std::string& key, const Bytes& value) {
  GlobalAllocatorGuard g;

  if (auto found = data_.find(key); found != data_.end()) {
    found->second = value;  // Copy
  } else {
    data_.insert(std::make_pair(key, value));  // Copy
  }
}

// Context: Server
std::optional<Bytes> PersistentStorage::TryGet(const std::string& key) const {
  if (auto found = data_.find(key); found != data_.end()) {
    return MakeCopy(found->second);  // Copy
  } else {
    return std::nullopt;
  }
}

void PersistentStorage::Delete(const std::string& key) {
  GlobalAllocatorGuard g;
  data_.erase(key);
}

// Context: Global
size_t PersistentStorage::ComputeDigest() const {
  DigestCalculator digest;
  // NB: sorted std::map, unique keys -> deterministic order
  for (const auto& [k, v] : data_) {
    digest.Eat(k).Eat(v.Raw());
  }
  return digest.Get();
}

}  // namespace whirl

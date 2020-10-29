#pragma once

namespace whirl {

// https://stackoverflow.com/questions/45507041/how-to-check-if-weak-ptr-is-empty-non-assigned

template <typename T>
bool IsInitialized(const std::weak_ptr<T>& ptr) {
  std::weak_ptr<T> empty;
  return ptr.owner_before(empty) || empty.owner_before(ptr);
}

// NB: Initialized and expired

template <typename T>
bool IsExpired(const std::weak_ptr<T>& ptr) {
  return IsInitialized(ptr) && ptr.expired();
}

}  // namespace whirl

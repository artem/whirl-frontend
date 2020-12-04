#pragma once

#include <utility>

namespace whirl {

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

template <typename T>
void HideToHeap(T&& obj) {
  new T(std::move(obj));
}

}  // namespace whirl

#pragma once

namespace whirl {

// Use at boundaries of process/global heaps

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

}  // namespace whirl

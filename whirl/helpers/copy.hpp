#pragma once

namespace whirl {

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

}  // namespace whirl

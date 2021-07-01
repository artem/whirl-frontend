#pragma once

#include <utility>

namespace whirl::matrix {

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

template <typename T>
T* MoveToHeap(T&& obj) {
  return new T(std::move(obj));
}

template <typename T, typename ... Args>
T* MakeStaticLikeObject(Args&& ... args) {
  return new T(std::forward<Args>(args)...);
}

}  // namespace whirl::matrix

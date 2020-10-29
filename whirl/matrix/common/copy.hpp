#pragma once

#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

// Use at boundaries of process/global heaps

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

template <typename T>
T CopyToHeap(const T& obj, Heap* heap) {
  HeapScopeGuard g(heap);
  T copy{obj};
  return copy;
}

}  // namespace whirl

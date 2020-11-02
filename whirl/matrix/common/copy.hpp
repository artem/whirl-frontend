#pragma once

#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

// Warning:
// Do not pass CoW-ed objects through heap boundaries
// Only objects that make deep copy in copy ctor are allowed
// Prefer primitive types, strings and structures

// Use at boundaries of process/global heaps

template <typename T>
T MakeCopy(const T& object) {
  T copy{object};
  return copy;
}

template <typename T>
T CopyToHeap(const T& object, Heap* heap) {
  HeapScopeGuard g(heap);
  T copy{object};
  return copy;
}

}  // namespace whirl

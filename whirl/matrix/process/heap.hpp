#pragma once

#include <whirl/matrix/common/heap.hpp>
#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

class ProcessHeap {
 public:
  HeapScopeGuard Use() {
    return HeapScopeGuard(&impl_);
  }

  size_t BytesAllocated() const {
    return impl_.BytesAllocated();
  }

  void Reset() {
    impl_.Reset();
  }

 private:
  Heap impl_;
};

}  // namespace whirl

#pragma once

#include <whirl/matrix/memory/heap.hpp>
#include <whirl/matrix/memory/allocator.hpp>

namespace whirl {

class ProcessHeap {
 public:
  HeapScopeGuard Use() {
    return HeapScopeGuard(&impl_);
  }

  size_t BytesAllocated() const {
    return impl_.BytesAllocated();
  }

  bool FromHere(char* addr) const {
    return impl_.FromHere(addr);
  }

  void Reset() {
    impl_.Reset();
  }

 private:
  Heap impl_;
};

}  // namespace whirl

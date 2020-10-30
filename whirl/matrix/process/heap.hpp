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

  bool FromHere(char* addr) const {
    return impl_.FromHere(addr);
  }

  void Reset() {
    impl_.Reset();
  }

  template <typename T, typename ... Args>
  T* New(Args&& ... args) {
    return new (impl_.Allocate(sizeof(T))) T(std::forward<Args>(args)...);
  }

 private:
  Heap impl_;
};

}  // namespace whirl

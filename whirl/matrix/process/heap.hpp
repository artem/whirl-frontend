#pragma once

#include <whirl/matrix/common/heap.hpp>
#include <whirl/matrix/common/allocator.hpp>

namespace whirl {

class ProcessHeap {
 public:
  HeapScope Use() {
    return HeapScope(&heap_);
  }

  size_t BytesAllocated() const {
    return heap_.BytesAllocated();
  }

  void Reset() {
    heap_.Reset();
  }

 private:
  Heap heap_;
};

}  // namespace whirl

#pragma once

#include <whirl/matrix/memory/allocator.hpp>

//////////////////////////////////////////////////////////////////////

void SetAllocator(whirl::MemoryAllocator* allocator);
whirl::MemoryAllocator* GetAllocator();

void PrintAllocDebugInfo();

uintptr_t GlobalAllocsCheckSum();

//////////////////////////////////////////////////////////////////////

class HeapScopeGuard {
 public:
  HeapScopeGuard(whirl::MemoryAllocator* a) {
    saved_ = GetAllocator();
    SetAllocator(a);
  }

  whirl::MemoryAllocator* ParentScopeHeap() {
    return saved_;
  }

  ~HeapScopeGuard() {
    SetAllocator(saved_);
  }

 private:
  whirl::MemoryAllocator* saved_;
};

//////////////////////////////////////////////////////////////////////

class GlobalHeapScope : public HeapScopeGuard {
 public:
  GlobalHeapScope() : HeapScopeGuard(nullptr) {
  }
};

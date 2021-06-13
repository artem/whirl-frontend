#pragma once

#include <whirl/matrix/memory/allocator.hpp>

//////////////////////////////////////////////////////////////////////

void SetAllocator(whirl::MemoryAllocator* allocator);
whirl::MemoryAllocator* GetAllocator();

void PrintAllocDebugInfo();

uintptr_t GlobalAllocsCheckSum();

//////////////////////////////////////////////////////////////////////

class AllocatorGuard {
 public:
  AllocatorGuard(whirl::MemoryAllocator* a) {
    saved_ = GetAllocator();
    SetAllocator(a);
  }

  whirl::MemoryAllocator* ParentScopeHeap() {
    return saved_;
  }

  ~AllocatorGuard() {
    SetAllocator(saved_);
  }

 private:
  whirl::MemoryAllocator* saved_;
};

//////////////////////////////////////////////////////////////////////

class GlobalAllocatorGuard : public AllocatorGuard {
 public:
  GlobalAllocatorGuard() : AllocatorGuard(nullptr) {
  }
};

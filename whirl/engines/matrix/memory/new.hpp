#pragma once

#include <whirl/engines/matrix/memory/allocator.hpp>

//////////////////////////////////////////////////////////////////////

// nullptr - global allocator
void SetAllocator(whirl::MemoryAllocator* allocator);
whirl::MemoryAllocator* GetAllocator();

//////////////////////////////////////////////////////////////////////

// Scope guards

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

class GlobalAllocatorGuard : public AllocatorGuard {
 public:
  GlobalAllocatorGuard() : AllocatorGuard(nullptr) {
  }
};

//////////////////////////////////////////////////////////////////////

uintptr_t GlobalAllocsCheckSum();

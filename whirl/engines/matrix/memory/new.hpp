#pragma once

#include <whirl/engines/matrix/memory/allocator.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// nullptr - global allocator
void SetAllocator(MemoryAllocator* allocator);
MemoryAllocator* GetAllocator();

//////////////////////////////////////////////////////////////////////

// Scope guards

class AllocatorGuard {
 public:
  AllocatorGuard(MemoryAllocator* a) {
    saved_ = GetAllocator();
    SetAllocator(a);
  }

  MemoryAllocator* ParentScopeHeap() {
    return saved_;
  }

  ~AllocatorGuard() {
    SetAllocator(saved_);
  }

 private:
  MemoryAllocator* saved_;
};

class GlobalAllocatorGuard : public AllocatorGuard {
 public:
  GlobalAllocatorGuard() : AllocatorGuard(nullptr) {
  }
};

//////////////////////////////////////////////////////////////////////

uintptr_t GlobalAllocsCheckSum();

}  // namespace whirl::matrix

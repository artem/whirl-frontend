#pragma once

#include <cstdlib>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct IMemoryAllocator {
  virtual ~IMemoryAllocator() = default;

  virtual void* Allocate(size_t bytes) = 0;
  virtual void Free(void* addr) = 0;
};

//////////////////////////////////////////////////////////////////////

// nullptr - global allocator
void SetAllocator(IMemoryAllocator* allocator);
IMemoryAllocator* GetAllocator();

//////////////////////////////////////////////////////////////////////

// Scope guards

class AllocatorGuard {
 public:
  AllocatorGuard(IMemoryAllocator* a) {
    saved_ = GetAllocator();
    SetAllocator(a);
  }

  IMemoryAllocator* ParentScopeHeap() {
    return saved_;
  }

  void RollBack() {
    if (active_) {
      SetAllocator(saved_);
      active_ = false;
    }
  }

  ~AllocatorGuard() {
    RollBack();
  }

 private:
  bool active_{true};
  IMemoryAllocator* saved_;
};

class GlobalAllocatorGuard : public AllocatorGuard {
 public:
  GlobalAllocatorGuard() : AllocatorGuard(nullptr) {
  }
};

//////////////////////////////////////////////////////////////////////

uintptr_t GlobalAllocsCheckSum();

}  // namespace whirl::matrix

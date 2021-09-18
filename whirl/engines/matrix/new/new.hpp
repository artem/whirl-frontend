#pragma once

#include <whirl/engines/matrix/new/allocator.hpp>

#include <cstdint>
#include <cstdlib>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// nullptr - global allocator
void SetAllocator(IMemoryAllocator* allocator);
IMemoryAllocator* GetCurrentAllocator();

//////////////////////////////////////////////////////////////////////

// Scope guards

class AllocatorGuard {
 public:
  AllocatorGuard(IMemoryAllocator* a) {
    saved_ = GetCurrentAllocator();
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

void ActivateAllocsTracker();
void PrintAllocsTrackerReport();

}  // namespace whirl::matrix

#pragma once

#include <whirl/matrix/memory/heap.hpp>

//////////////////////////////////////////////////////////////////////

void SetHeap(whirl::Heap* heap);
whirl::Heap* GetHeap();

void PrintAllocDebugInfo();

//////////////////////////////////////////////////////////////////////

class HeapScopeGuard {
 public:
  HeapScopeGuard(whirl::Heap* h) {
    saved_ = GetHeap();
    SetHeap(h);
  }

  whirl::Heap* ParentScopeHeap() {
    return saved_;
  }

  ~HeapScopeGuard() {
    SetHeap(saved_);
  }

 private:
  whirl::Heap* saved_;
};

//////////////////////////////////////////////////////////////////////

class GlobalHeapScope : public HeapScopeGuard {
 public:
  GlobalHeapScope() : HeapScopeGuard(nullptr) {
  }
};

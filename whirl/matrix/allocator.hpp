#pragma once

#include <whirl/matrix/heap.hpp>

//////////////////////////////////////////////////////////////////////

void SetHeap(whirl::Heap* heap);
whirl::Heap* GetHeap();

//////////////////////////////////////////////////////////////////////

class HeapScope {
 public:
  HeapScope(whirl::Heap* h) {
    saved_ = GetHeap();
    SetHeap(h);
  }

  ~HeapScope() {
    SetHeap(saved_);
  }

 private:
  whirl::Heap* saved_;
};

//////////////////////////////////////////////////////////////////////

class GlobalHeapScope : public HeapScope {
 public:
  GlobalHeapScope() : HeapScope(nullptr) {
  }
};

#include <whirl/matrix/common/allocator.hpp>

#include <new>

static thread_local whirl::Heap* heap{nullptr};

static int global_allocs_balance = 0;

void SetHeap(whirl::Heap* h) {
  heap = h;
}

whirl::Heap* GetHeap() {
  return heap;
}

void* operator new(size_t size) {
  if (heap) {
    return heap->Allocate(size);
  }

  if (void* addr = std::malloc(size)) {
    ++global_allocs_balance;
    return addr;
  } else {
    throw std::bad_alloc{};
  }
}

void operator delete(void* addr) noexcept {
  if (heap) {
    heap->Free((char*)addr);
    return;
  }

  std::free(addr);
  --global_allocs_balance;
}

void PrintAllocDebugInfo() {
  printf("Malloc/free balance: %d\n", global_allocs_balance);
}

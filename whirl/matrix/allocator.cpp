#include <whirl/matrix/allocator.hpp>

#include <new>

static thread_local whirl::Heap* heap{nullptr};

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
}

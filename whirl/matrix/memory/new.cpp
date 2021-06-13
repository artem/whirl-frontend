#include <whirl/matrix/memory/new.hpp>

#include <cstdlib>
#include <cstdio>
#include <new>

//////////////////////////////////////////////////////////////////////

static thread_local whirl::MemoryAllocator* allocator{nullptr};

void SetAllocator(whirl::MemoryAllocator* a) {
  allocator = a;
}

whirl::MemoryAllocator* GetAllocator() {
  return allocator;
}

//////////////////////////////////////////////////////////////////////

static uintptr_t global_allocs_checksum = 0;

void* operator new(size_t size) {
  if (allocator != nullptr) {
    return allocator->Allocate(size);
  }

  if (void* addr = std::malloc(size)) {
    global_allocs_checksum ^= (uintptr_t)addr;
    return addr;
  } else {
    throw std::bad_alloc{};
  }
}

void operator delete(void* addr) noexcept {
  if (allocator != nullptr) {
    allocator->Free(addr);
    return;
  }

  std::free(addr);
  global_allocs_checksum ^= (uintptr_t)addr;
}

uintptr_t GlobalAllocsCheckSum() {
  return global_allocs_checksum;
}

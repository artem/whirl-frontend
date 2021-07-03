#include <whirl/engines/matrix/memory/new.hpp>

#include <wheels/support/panic.hpp>

#include <cstdlib>
#include <cstdio>
#include <new>

//////////////////////////////////////////////////////////////////////

static thread_local whirl::matrix::IMemoryAllocator* allocator{nullptr};

namespace whirl::matrix {

void SetAllocator(IMemoryAllocator* a) {
  allocator = a;
}

IMemoryAllocator* GetAllocator() {
  return allocator;
}

}  // namespace whirl::matrix

//////////////////////////////////////////////////////////////////////

static uintptr_t global_allocs_checksum = 0;

static void* AllocateGlobal(size_t size) {
  if (void* addr = std::malloc(size)) {
    global_allocs_checksum ^= (uintptr_t)addr;
    return addr;
  } else {
    WHEELS_PANIC("Failed to malloc " << size << " bytes");
  }
}

static void FreeGlobal(void* addr) {
  std::free(addr);
  global_allocs_checksum ^= (uintptr_t)addr;
}

namespace whirl::matrix {

uintptr_t GlobalAllocsCheckSum() {
  return global_allocs_checksum;
}

}  // namespace whirl::matrix

//////////////////////////////////////////////////////////////////////

void* operator new(size_t size) {
  if (allocator != nullptr) {
    return allocator->Allocate(size);
  }
  return AllocateGlobal(size);
}

void operator delete(void* addr) noexcept {
  if (allocator != nullptr) {
    allocator->Free(addr);
    return;
  }
  return FreeGlobal(addr);
}

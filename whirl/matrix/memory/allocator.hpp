#pragma once

#include <wheels/support/mmap_allocation.hpp>

#include <cstdlib>

namespace whirl {

class MemoryAllocator {
 public:
  MemoryAllocator();
  ~MemoryAllocator();

  // And initialize with zeroes
  void* Allocate(size_t bytes);

  void Free(void* addr);

  bool FromHere(void* addr) const;

  void Reset();

  size_t BytesAllocated() const;

 private:
  void ZeroFillTo(char* pos);
  char* AllocateNewBlock(size_t bytes);
  bool Overflow(size_t bytes) const;
  static char* WriteBlockHeader(char* addr, size_t size);

 private:
  wheels::MmapAllocation heap_;
  char* next_;
  char* zfilled_;
};

}  // namespace whirl
#pragma once

#include <wheels/support/mmap_allocation.hpp>
#include <wheels/support/assert.hpp>

#include <map>
#include <utility>
#include <optional>

namespace whirl {

//////////////////////////////////////////////////////////////////////

wheels::MmapAllocation AllocateHeapMemory();
void ReleaseHeapMemory(wheels::MmapAllocation heap);

//////////////////////////////////////////////////////////////////////

class Block {
 public:
  char* UserAddr() const {
    return addr_ + 8;
  }

  char* End() const {
    return UserAddr() + Size();
  }

  size_t Size() const {
    return *reinterpret_cast<size_t*>(addr_);
  }

  static Block FromUserAddr(char* user_addr) {
    return Block(user_addr - 8);
  }

 private:
  Block(char* real_addr) : addr_(real_addr) {
  }

 private:
  char* addr_;
};

//////////////////////////////////////////////////////////////////////

// Currently: naive bump-pointer allocator

class Heap {
 public:
  Heap() : heap_(AllocateHeapMemory()), next_(heap_.Start()) {
  }

  ~Heap() {
    ReleaseHeapMemory(std::move(heap_));
  }

  char* Allocate(size_t bytes) {
    return AllocateNewBlock(bytes);
  }

  void Free(char* /*addr*/) {
    // ...
  }

  size_t BytesAllocated() const {
    return next_ - heap_.Start();
  }

  void Reset() {
    next_ = heap_.Start();
  }

 private:
  char* AllocateNewBlock(size_t bytes) {
    WHEELS_VERIFY(next_ + 8 + bytes < heap_.End(), "Heap overflow");
    // printf("Allocate %zu bytes\n", bytes);
    char* user_addr = WriteBlockHeader(next_, bytes);
    next_ = user_addr + bytes;
    return user_addr;
  }

  static char* WriteBlockHeader(char* addr, size_t size) {
    *reinterpret_cast<size_t*>(addr) = size;
    return addr + 8;
  };

 private:
  wheels::MmapAllocation heap_;
  char* next_;
};

}  // namespace whirl

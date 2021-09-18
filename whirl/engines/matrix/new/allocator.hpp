#pragma once

#include <cstdlib>

namespace whirl::matrix {

struct IMemoryAllocator {
  virtual ~IMemoryAllocator() = default;

  virtual void* Allocate(size_t bytes) = 0;
  virtual void Free(void* addr) = 0;
};

}  // namespace whirl::matrix

#pragma once

#include <cstdlib>

namespace whirl::matrix {

struct IMemoryAllocator {
  virtual ~IMemoryAllocator() = default;

  virtual void* Allocate(size_t bytes) = 0;
  virtual void Free(void* address) = 0;
};

}  // namespace whirl::matrix

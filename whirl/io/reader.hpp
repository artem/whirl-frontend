#pragma once

#include <wheels/support/memspan.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

struct IReader {
  virtual ~IReader() = default;

  virtual size_t ReadSome(wheels::MutableMemView buffer) = 0;
};

//////////////////////////////////////////////////////////////////////

void ReadFull(IReader* reader, wheels::MutableMemView buffer);

std::string ReadString(IReader* reader, size_t length);

}  // namespace whirl

#pragma once

#include <wheels/support/memspan.hpp>
#include <wheels/support/result.hpp>

namespace whirl {

struct IWriter {
  virtual ~IWriter() = default;

  virtual size_t Write(wheels::ConstMemView data) = 0;
};

void WriteAll(IWriter* writer, wheels::ConstMemView data);

}  // namespace whirl

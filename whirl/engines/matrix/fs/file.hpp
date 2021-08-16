#pragma once

#include <wheels/support/memspan.hpp>

#include <vector>

namespace whirl::matrix::fs {

class File {
 public:
  size_t Size() const;
  void Append(wheels::ConstMemView data);
  size_t PRead(size_t offset, wheels::MutableMemView buffer) const;

  size_t ComputeDigest() const;

 private:
  std::vector<char> data_;
};

}  // namespace whirl::matrix::fs

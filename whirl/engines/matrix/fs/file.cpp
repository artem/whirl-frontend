#include <whirl/engines/matrix/fs/file.hpp>

#include <wheels/io/memory.hpp>

#include <wheels/memory/view_of.hpp>
#include <wheels/support/hash.hpp>

namespace whirl::matrix::fs {

size_t File::Size() const {
  return data_.size();
}

void File::Append(wheels::ConstMemView append) {
  size_t curr_size = data_.size();
  size_t new_size = curr_size + append.Size();

  // Write
  data_.resize(new_size);
  memcpy(/*to=*/&data_[curr_size], /*from=*/append.Data(), append.Size());
}

wheels::ConstMemView File::Tail(size_t offset) const {
  if (offset >= data_.size()) {
    return {nullptr, 0};
  }
  auto tail = wheels::ViewOf(data_);
  tail += offset;
  return tail;
}

size_t File::PRead(size_t offset, wheels::MutableMemView buffer) const {
  wheels::io::MemoryReader reader(Tail(offset));
  return reader.Read(buffer);
}

size_t File::ComputeDigest() const {
  return wheels::HashRange(17, wheels::ViewOf(data_));
}

}  // namespace whirl::matrix::fs

#include <whirl/engines/matrix/fs/file.hpp>

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

size_t File::PRead(size_t offset, wheels::MutableMemView buffer) const {
  if (offset >= data_.size()) {
    return 0;
  }
  // offset < meta.size
  size_t tail_size = data_.size() - offset;
  size_t to_read = std::min(tail_size, buffer.Size());

  // Read
  memcpy(/*to=*/buffer.Data(), /*from=*/&data_[offset], to_read);

  return to_read;
}

size_t File::ComputeDigest() const {
  return data_.size();  // TODO
}

}  // namespace whirl::matrix::fs

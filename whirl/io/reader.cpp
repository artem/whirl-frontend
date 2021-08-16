#include <whirl/io/reader.hpp>

namespace whirl {

size_t Read(IReader* reader, wheels::MutableMemView buffer) {
  size_t total_bytes_read = 0;

  while (buffer.HasSpace()) {
    const size_t bytes_read = reader->ReadSome(buffer);
    if (bytes_read == 0) {
      break;
    }
    buffer += bytes_read;
    total_bytes_read += bytes_read;
  }

  return total_bytes_read;
}

void ReadFull(IReader* reader, wheels::MutableMemView buffer) {
  while (buffer.HasSpace()) {
    const size_t bytes_read = reader->ReadSome(buffer);
    if (bytes_read == 0) {
      throw std::runtime_error("Eof");
    }
    buffer += bytes_read;
  }
}

std::string ReadString(IReader* reader, size_t length) {
  std::string str(length, '?');
  ReadFull(reader, {str.data(), length});
  return str;
}

}  // namespace whirl

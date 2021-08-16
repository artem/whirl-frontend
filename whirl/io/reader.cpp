#include <whirl/io/reader.hpp>

namespace whirl {

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

#include <whirl/io/writer.hpp>

namespace whirl {

void WriteAll(IWriter* writer, wheels::ConstMemView data) {
  while (data.Size() > 0) {
    size_t bytes_written = writer->Write(data);
    data += bytes_written;
  }
}

}  // namespace whirl

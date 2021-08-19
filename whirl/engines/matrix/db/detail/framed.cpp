#include <whirl/engines/matrix/db/detail/framed.hpp>

#include <wheels/io/read.hpp>
#include <wheels/io/write.hpp>
#include <wheels/io/limit.hpp>

#include <wheels/support/mem_view_of.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

std::optional<std::string> FramedReader::ReadNextFrame() {
  auto header = ReadNextFrameHeader();
  if (!header) {
    return std::nullopt;
  }
  wheels::io::LimitReader frame_reader(reader_, header->size);
  auto frame = wheels::io::ReadAll(&frame_reader);
  WHEELS_VERIFY(frame.length() == header->size, "Cannot read next frame");
  return frame;
}

std::optional<detail::FrameHeader> FramedReader::ReadNextFrameHeader() {
  detail::FrameHeader header;
  size_t bytes_read = wheels::io::Read(reader_, detail::MutViewOf(header));
  if (bytes_read == 0) {
    return std::nullopt;
  }
  if (bytes_read < sizeof(detail::FrameHeader)) {
    WHEELS_PANIC("Broken frame header");
  }
  return header;
}

//////////////////////////////////////////////////////////////////////

void FramedWriter::WriteFrame(const std::string& frame) {
  WriteFrameHeader(frame);
  writer_->Write(wheels::ViewOf(frame));
}

void FramedWriter::WriteFrameHeader(const std::string& frame) {
  detail::FrameHeader header{frame.length()};
  writer_->Write(detail::MutViewOf(header));
}

}  // namespace whirl
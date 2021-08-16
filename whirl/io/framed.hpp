#pragma once

#include <whirl/io/reader.hpp>
#include <whirl/io/writer.hpp>

#include <wheels/support/noncopyable.hpp>

#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

namespace detail {

struct FrameHeader {
  uint64_t size;
};

inline wheels::MutableMemView AsBytes(FrameHeader& h) {
  return {(char*)&h, sizeof(FrameHeader)};
}

}  // namespace detail

//////////////////////////////////////////////////////////////////////

class FramedReader : private wheels::NonCopyable {
 public:
  explicit FramedReader(IReader* reader) : reader_(reader) {
  }

  std::optional<std::string> ReadNextFrame() {
    auto header = ReadNextFrameHeader();
    if (!header) {
      return std::nullopt;
    }
    auto frame = ReadString(reader_, header->size);
    return frame;
  }

 private:
  std::optional<detail::FrameHeader> ReadNextFrameHeader() {
    detail::FrameHeader header;
    // TODO: without exceptions!
    try {
      ReadFull(reader_, detail::AsBytes(header));
      return header;
    } catch (...) {
      return std::nullopt;
    }
  }

 private:
  IReader* reader_;
};

//////////////////////////////////////////////////////////////////////

class FramedWriter : private wheels::NonCopyable {
 public:
  explicit FramedWriter(IWriter* writer) : writer_(writer) {
  }

  void WriteFrame(const std::string& frame) {
    WriteFrameHeader(frame);
    WriteAll(writer_, {frame.data(), frame.length()});
  }

 private:
  void WriteFrameHeader(const std::string& frame) {
    detail::FrameHeader header{frame.length()};
    WriteAll(writer_, detail::AsBytes(header));
  }

 private:
  IWriter* writer_;
};

}  // namespace whirl

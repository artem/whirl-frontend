#pragma once

#include <wheels/io/reader.hpp>
#include <wheels/io/writer.hpp>

#include <wheels/support/noncopyable.hpp>

#include <optional>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

namespace detail {

struct FrameHeader {
  uint64_t size;
};

inline wheels::MutableMemView MutViewOf(FrameHeader& h) {
  return {(char*)&h, sizeof(FrameHeader)};
}

}  // namespace detail

//////////////////////////////////////////////////////////////////////

class FramedReader : private wheels::NonCopyable {
 public:
  explicit FramedReader(wheels::io::IReader* reader) : reader_(reader) {
  }

  std::optional<std::string> ReadNextFrame();

 private:
  std::optional<detail::FrameHeader> ReadNextFrameHeader();

 private:
  wheels::io::IReader* reader_;
};

//////////////////////////////////////////////////////////////////////

class FramedWriter : private wheels::NonCopyable {
 public:
  explicit FramedWriter(wheels::io::IWriter* writer) : writer_(writer) {
  }

  void WriteFrame(const std::string& frame);

 private:
  void WriteFrameHeader(const std::string& frame);

 private:
  wheels::io::IWriter* writer_;
};

}  // namespace whirl

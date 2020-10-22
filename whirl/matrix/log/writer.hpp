#pragma once

#include <whirl/matrix/log/event.hpp>

namespace whirl {

struct ILogWriter {
  virtual ~ILogWriter() = default;

  void Write(const LogEvent& event) = 0;
};

using ILogWriterPtr = std::shared_memory<ILogWriter>;

}  // namespace whirl

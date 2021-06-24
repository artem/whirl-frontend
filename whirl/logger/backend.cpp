#include <whirl/logger/backend.hpp>

namespace whirl {

ILoggerBackend* gbackend = nullptr;

void SetLoggerBackend(ILoggerBackend* backend) {
  gbackend = backend;
}

ILoggerBackend* GetLoggerBackend() {
  return gbackend;
}

}  // namespace whirl
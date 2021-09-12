#include <whirl/logger/backend.hpp>

namespace whirl {

LoggerBackendAccessor gaccessor;

void SetLoggerBackend(LoggerBackendAccessor accessor) {
  gaccessor = accessor;
}

ILoggerBackend* GetLoggerBackend() {
  return gaccessor();
}

}  // namespace whirl

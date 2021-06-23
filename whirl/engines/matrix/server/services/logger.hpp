#pragma once

#include <whirl/services/logger.hpp>

#include <whirl/engines/matrix/log/logger.hpp>

#include <wheels/support/panic.hpp>

namespace whirl {

class LoggerProxy : public INodeLogger {
 public:
  // Context: Server
  void Log(NodeLogLevel level, const std::string& message) override {
    impl_.Log(Translate(level), message);
  }

 private:
  static LogLevel Translate(NodeLogLevel level) {
    switch (level) {
      case NodeLogLevel::Debug: return LogLevel::Debug;
      case NodeLogLevel::Info: return LogLevel::Info;
      case NodeLogLevel::Error: return LogLevel::Error;
      default: WHEELS_PANIC("Node log level not supported");
    }
  }

 private:
  Logger impl_{"Node"};
};

}  // namespace whirl

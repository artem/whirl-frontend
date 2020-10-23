#pragma once

#include <whirl/services/logger.hpp>

#include <whirl/matrix/log/logger.hpp>

namespace whirl {

class LoggerProxy : public INodeLogger {
 public:
  // Context: Server
  void Log(const std::string& message) {
    impl_.Log(message);
  }

 private:
  Logger impl_{"Node"};
};

}  // namespace whirl

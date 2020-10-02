#pragma once

#include <string>

#include <whirl/matrix/global.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class Logger {
 public:
  Logger(const std::string& component);

  void Log(const std::string& message);

 private:
  std::string component_;
};

//////////////////////////////////////////////////////////////////////

void LogMessage(std::string message);

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

#define WHIRL_LOG(message) \
  ::whirl::LogMessage(::wheels::StringBuilder() << message);

#define WHIRL_LLOG(message) logger_.Log(::wheels::StringBuilder() << message);

#else

#define WHIRL_LOG(message) ;

#define WHIRL_LLOG(message)

#endif

}  // namespace whirl

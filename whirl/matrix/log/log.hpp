#pragma once

#include <string>

#include <whirl/matrix/world/global.hpp>

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

#ifndef NDEBUG

#define WHIRL_LOG(message) logger_.Log(::wheels::StringBuilder() << message);

#else

#define WHIRL_LOG(message) ;

#endif

}  // namespace whirl

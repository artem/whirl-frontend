#pragma once

#include <system_error>

namespace whirl {

inline std::error_code TimeOutError() {
  return std::make_error_code(std::errc::timed_out);
}

}  // namespace whirl
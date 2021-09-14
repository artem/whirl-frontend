#pragma once

#include <whirl/engines/matrix/log/event.hpp>

#include <iostream>

namespace whirl::matrix::log {

void FormatLogEventTo(const LogEvent& event, std::ostream& out);

}  // namespace whirl::matrix::log

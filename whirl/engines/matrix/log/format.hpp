#pragma once

#include <whirl/engines/matrix/log/event.hpp>

#include <iostream>

namespace whirl::matrix {

void FormatLogEventTo(const LogEvent& event, std::ostream& out);

}  // namespace whirl::matrix

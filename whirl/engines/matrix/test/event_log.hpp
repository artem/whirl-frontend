#pragma once

#include <whirl/engines/matrix/log/event.hpp>

#include <iostream>

namespace whirl::matrix {

void WriteTextLog(const EventLog& log, std::ostream& out);

}  // namespace whirl::matrix

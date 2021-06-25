#include <whirl/engines/matrix/log/format.hpp>

#include <iomanip>
#include <sstream>

namespace whirl::matrix {

// Formatting

static std::string_view LimitWidth(std::string_view str, size_t width) {
  if (str.length() < width) {
    return str;
  } else {
    return str.substr(width);
  }
}

#define _FMT(str, width) std::setw(width) << LimitWidth(str, width)

void FormatLogEventTo(const LogEvent& event, std::ostream& out) {
  out << std::left
      << "[T " << event.time << " | " << event.step << ']'
      << '\t'
      << '[' << _FMT(LogLevelToString(event.level), 7) << ']'
      << '\t'
      << '[' << _FMT(event.actor, 15) << ']'
      << '\t'
      << '[' << _FMT(event.component, 12) << ']';

  if (event.trace_id.has_value()) {
    out << "\t"
        << "[" << _FMT(event.trace_id.value(), 6) << "]";
  }

  out << "\t" << event.message;
}

std::string LogEventToString(const LogEvent& event) {
  std::ostringstream out;
  FormatLogEventTo(event, out);
  return out.str();
}

}  // namespace whirl::matrix

#include <whirl/matrix/log/log.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Formatting

static std::string ToWidth(const std::string& s, size_t width) {
  if (s.length() > width) {
    return s.substr(0, width);
  }
  return s + std::string(width - s.length(), ' ');
}

//////////////////////////////////////////////////////////////////////

void Log::WriteTo(const LogEvent& event, std::ostream& out) {
  out << "[T " << event.time << " | " << event.step << "]"
      << "\t"
      << "[" << ToWidth(event.actor, 15) << "]"
      << "\t"
      << "[" << ToWidth(event.component, 12) << "]";

  if (event.trace_id.has_value()) {
    out << "\t"
        << "[" << ToWidth(event.trace_id.value(), 6) << "]";
  }

  out << "\t" << event.message;
}

}  // namespace whirl

#include <whirl/engines/matrix/test/event_log.hpp>

#include <whirl/engines/matrix/log/format.hpp>

namespace whirl::matrix {

void WriteTextLog(const log::EventLog& events, std::ostream& out) {
  static const size_t kHeadLines = 100;
  static const size_t kTailLines = 256;
  static const size_t kLinesLimit = kHeadLines + kTailLines;

  if (events.size() <= kLinesLimit) {
    // Full log
    for (const auto& event : events) {
      FormatLogEventTo(event, out);
      out << std::endl;
    }
  } else {
    // Too long

    // Head
    for (size_t i = 0; i < kHeadLines; ++i) {
      FormatLogEventTo(events[i], out);
      out << std::endl;
    }

    // Skip
    size_t lines_skipped = events.size() - kLinesLimit;
    out << "... (" << lines_skipped << " lines skipped)" << std::endl;

    // Tail
    for (size_t i = events.size() - kTailLines; i < events.size(); ++i) {
      FormatLogEventTo(events[i], out);
      out << std::endl;
    }
  }
}

}  // namespace whirl::matrix
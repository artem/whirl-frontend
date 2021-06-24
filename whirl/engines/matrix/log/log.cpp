#include <whirl/engines/matrix/log/log.hpp>

#include <whirl/logger/enabled.hpp>

#include <whirl/engines/matrix/memory/new.hpp>

#include <wheels/support/assert.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

#include <await/fibers/core/api.hpp>
#include <whirl/rpc/trace.hpp>

#include <iostream>

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

static void WriteTo(const LogEvent& event, std::ostream& out) {
  out << "[T " << event.time << " | " << event.step << "]"
      << "\t"
      << "[" << ToWidth(LogLevelToString(event.level), 7) << "]"
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

//////////////////////////////////////////////////////////////////////

static std::string ThisFiberName() {
  auto name = await::fibers::self::GetName();
  if (name.has_value()) {
    return *name;
  } else {
    return wheels::StringBuilder() << "T" << await::fibers::self::GetId();
  }
}

static std::string ThisActorName() {
  if (AmIActor()) {
    return CurrentActorName();
  } else {
    return "World";
  }
}

static std::string DescribeThisActor() {
  std::string actor = ThisActorName();
  if (await::fibers::AmIFiber()) {
    actor = actor + " /" + ThisFiberName();
  }
  return actor;
}

//////////////////////////////////////////////////////////////////////

LogBackend::LogBackend() {
#if defined(WHIRL_LOGGING_ENABLED)
  file_ = GetLogFile();
#endif
  InitLevels();
}

void LogBackend::Write(const LogEvent& event) {
  std::ostringstream event_out;
  WriteTo(event, event_out);
  std::string event_str = event_out.str();

  memory_ << event_str << std::endl;

#if defined(WHIRL_LOGGING_ENABLED)
  file_ << event_str << std::endl;
#endif
}

//////////////////////////////////////////////////////////////////////

static LogLevel kDefaultMinLogLevel = LogLevel::Info;

void LogBackend::InitLevels() {
  levels_ = GetLogLevelsFromEnv();
}

LogLevel LogBackend::GetMinLevel(const std::string& component) const {
  if (auto it = levels_.find(component); it != levels_.end()) {
    return it->second;
  }
  return kDefaultMinLogLevel;
}

void LogBackend::Log(const std::string& component, LogLevel level,
                     const std::string& message) {
  GlobalAllocatorGuard g;
  Write(MakeEvent(component, level, message));
}

LogEvent LogBackend::MakeEvent(const std::string& component, LogLevel level,
                               const std::string& message) const {
  LogEvent event;

  event.time = GlobalNow();
  event.step = WorldStepNumber();
  event.level = level;
  event.actor = DescribeThisActor();
  event.component = component;
  event.trace_id = rpc::TryGetCurrentTraceId();
  event.message = message;

  return event;
}

}  // namespace whirl

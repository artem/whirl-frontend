#include <whirl/matrix/log/logger.hpp>
#include <whirl/matrix/log/event.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/fibers/core/api.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl {

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

LogEvent Logger::MakeEvent(const std::string& message) const {
  LogEvent event;

  event.time = GlobalNow();
  event.step = WorldStepNumber();
  event.actor = DescribeThisActor();
  event.component = component_;
  event.trace_id = rpc::TryGetCurrentTraceId();
  event.message = message;

  return event;
}

void Logger::Write(const LogEvent& event) {
  GetLog().Write(event);
}

Logger::Logger(const std::string& component) : component_(component) {
}

void Logger::Log(const std::string& message) {
  GlobalHeapScope guard;
  Write(MakeEvent(message));
}

}  // namespace whirl
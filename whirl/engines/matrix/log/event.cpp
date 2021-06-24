#include <whirl/engines/matrix/log/event.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

#include <whirl/rpc/trace.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Current context

static std::string ThisFiberServiceName() {
  return wheels::StringBuilder() << "T" << await::fibers::self::GetId();
}

static std::string ThisFiberName() {
  auto name = await::fibers::self::GetName();
  if (name.has_value()) {
    return *name;
  } else {
    return ThisFiberServiceName();
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
  wheels::StringBuilder descr;

  descr << ThisActorName();
  if (await::fibers::AmIFiber()) {
    descr << " /" << ThisFiberName();
  }
  return descr;
}

//////////////////////////////////////////////////////////////////////

LogEvent MakeLogEvent(const std::string& component, LogLevel level,
                      const std::string& message) {
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
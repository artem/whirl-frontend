#include <whirl/engines/matrix/log/event.hpp>

#include <whirl/engines/matrix/world/global/global.hpp>

#include <whirl/rpc/trace.hpp>

#include <await/fibers/core/api.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// Current context

static std::string ThisFiberServiceName() {
  return wheels::StringBuilder() << 'T' << await::fibers::self::GetId();
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
    return ThisActor()->Name();
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

LogEvent MakeLogEvent(const timber::Event& e) {
  LogEvent event;

  event.time = GlobalNow();
  event.step = WorldStepNumber();
  event.level = e.level;
  event.actor = DescribeThisActor();
  event.component = e.component;
  event.trace_id = rpc::TryGetCurrentTraceId();
  event.message = e.message;

  return event;
}

}  // namespace whirl::matrix
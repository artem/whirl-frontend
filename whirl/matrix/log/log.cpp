#include <whirl/matrix/log/log.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/fibers/core/api.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <wheels/support/string_builder.hpp>

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

static std::string ThisFiberName() {
  auto name = await::fibers::GetName();
  if (name.has_value()) {
    return *name;
  } else {
    return wheels::StringBuilder() << "T" << await::fibers::GetId();
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

Logger::Logger(const std::string& component) : component_(component) {
}

void Logger::Log(const std::string& message) {
  GlobalHeapScope guard;

  std::stringstream event_out;

  event_out << "[T " << GlobalNow() << " | " << WorldStepNumber() << "]"
            << "\t"
            << "[" << ToWidth(DescribeThisActor(), 15) << "]"
            << "\t"
            << "[" << ToWidth(component_, 12) << "]";

  if (auto rpc_trace_id = rpc::TryGetCurrentTraceId()) {
    event_out << "\t" << "[" << ToWidth(rpc_trace_id.value(), 6) << "]";
  }

  event_out << "\t" << message;

  std::cout << event_out.str() << std::endl;
}

}  // namespace whirl

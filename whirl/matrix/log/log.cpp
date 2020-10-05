#include <whirl/matrix/log/log.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/fibers/core/api.hpp>
#include <whirl/rpc/impl/trace.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Helpers

static std::string ToWidth(std::string& s, size_t width) {
  if (s.length() > width) {
    return s.substr(0, width);
  }
  return s + std::string(width - s.length(), ' ');
}

static std::string ThisFiberName() {
  auto name = await::fibers::GetName();
  if (name.has_value()) {
    return *name;
  } else {
    return wheels::StringBuilder() << "T" << await::fibers::GetId();
  }
}

std::string ThisActorName() {
  if (AmIActor()) {
    return CurrentActorName();
  } else {
    return "World";
  }
}

//////////////////////////////////////////////////////////////////////

Logger::Logger(const std::string& component) : component_(component) {
}

void Logger::Log(const std::string& message) {
  GlobalHeapScope guard;

  std::string actor = ThisActorName();

  if (await::fibers::AmIFiber()) {
    actor = actor + " /" + ThisFiberName();
  }

  std::stringstream event_out;

  auto safe_message = message;
  event_out << "[T " << GlobalNow() << " | " << WorldStepNumber() << "]"
            << "\t"
            << "[" << ToWidth(actor, 15) << "]"
            << "\t"
            << "[" << ToWidth(component_, 12) << "]";

  if (await::fibers::AmIFiber()) {
    if (auto trace_id = rpc::GetCurrentTraceId()) {
      event_out << "\t" << "[" << ToWidth(trace_id.value(), 5) << "]";
    }
  }

  event_out << "\t" << safe_message;

  std::cout << event_out.str() << std::endl;
}

}  // namespace whirl

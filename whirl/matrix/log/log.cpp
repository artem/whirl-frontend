#include <whirl/matrix/log/log.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/world/global.hpp>

#include <await/fibers/core/api.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

static std::string ToWidth(std::string& s, size_t width) {
  if (s.length() > width) {
    return s.substr(0, width);
  }
  return s + std::string(width - s.length(), ' ');
}

//////////////////////////////////////////////////////////////////////

Logger::Logger(const std::string& component) : component_(component) {
}

void Logger::Log(const std::string& message) {
  GlobalHeapScope guard;

  std::string actor;
  if (AmIActor()) {
    actor = CurrentActorName();
  } else {
    actor = "World";
  }

  if (await::fibers::AmIFiber()) {
    actor = actor + " /T" + std::to_string(await::fibers::GetId());
  }

  auto safe_message = message;
  std::cout << "[T " << GlobalNow() << " | " << WorldStepNumber() << "]"
            << "\t"
            << "[" << ToWidth(actor, 15) << "]"
            << "\t"
            << "[" << ToWidth(component_, 12) << "]"
            << "\t" << safe_message << std::endl;
}

}  // namespace whirl

#include <whirl/matrix/history/recorder.hpp>

#include <whirl/matrix/world/global.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

Call RunningCall::CompleteWith(Value result) {
  return Call{method, arguments, result, start_time, GlobalNow()};
}

Call RunningCall::MaybeCompleted() {
  return Call{method, arguments, Value::Void(), start_time, std::nullopt};
}

//////////////////////////////////////////////////////////////////////

Recorder::Cookie Recorder::CallStarted(const std::string& method,
                             const std::string& input) {
  GlobalHeapScope g;

  Cookie id = ++next_cookie;
  running_calls_.emplace(id,
                         RunningCall{method, Arguments{input}, GlobalNow()});
  return id;
}

void Recorder::CallCompleted(Cookie id, const std::string& output) {
  GlobalHeapScope g;

  auto it = running_calls_.find(id);

  auto pending_call = std::move(it->second);
  running_calls_.erase(it);

  completed_calls_.push_back(pending_call.CompleteWith(output));
}

void Recorder::CallMaybeCompleted(Cookie id) {
  GlobalHeapScope g;

  auto it = running_calls_.find(id);

  auto pending_call = std::move(it->second);
  running_calls_.erase(it);

  completed_calls_.push_back(pending_call.MaybeCompleted());
}

void Recorder::Remove(Cookie id) {
  GlobalHeapScope g;

  running_calls_.erase(id);
}

void Recorder::Stop() {
  for (auto& [_, call] : running_calls_) {
    completed_calls_.push_back(call.MaybeCompleted());
  }
}

}  // namespace whirl::histories

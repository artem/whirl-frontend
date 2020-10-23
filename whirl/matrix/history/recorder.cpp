#include <whirl/matrix/history/recorder.hpp>

#include <whirl/matrix/world/global.hpp>

#include <whirl/matrix/common/allocator.hpp>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

Call Recorder::Complete(const RunningCall& call, Value result) {
  return Call{call.method,     call.arguments, result,
              call.start_time, GlobalNow(),    call.labels};
}

Call Recorder::Lost(const RunningCall& call) {
  return Call{call.method,     call.arguments, Value::Void(),
              call.start_time, std::nullopt,   call.labels};
}

//////////////////////////////////////////////////////////////////////

Recorder::Cookie Recorder::CallStarted(const std::string& method,
                                       const std::string& input) {
  GlobalHeapScope g;

  Cookie id = ++next_id_;
  running_calls_.emplace(
      id, RunningCall{method, Arguments{input}, GlobalNow(), {}});
  return id;
}

void Recorder::AddLabel(Cookie id, const std::string& label) {
  GlobalHeapScope g;

  auto it = running_calls_.find(id);
  WHEELS_VERIFY(it != running_calls_.end(), "Call not found");
  it->second.labels.push_back(label);
}

void Recorder::CallCompleted(Cookie id, const std::string& output) {
  GlobalHeapScope g;

  auto it = running_calls_.find(id);

  auto pending_call = std::move(it->second);
  running_calls_.erase(it);

  finalized_calls_.push_back(Complete(pending_call, output));
}

void Recorder::CallLost(Cookie id) {
  GlobalHeapScope g;

  auto it = running_calls_.find(id);

  auto pending_call = std::move(it->second);
  running_calls_.erase(it);

  finalized_calls_.push_back(Lost(pending_call));
}

void Recorder::RemoveCall(Cookie id) {
  GlobalHeapScope g;

  running_calls_.erase(id);
}

size_t Recorder::NumCompletedCalls() const {
  size_t count = 0;
  for (const auto& call : finalized_calls_) {
    if (call.IsCompleted()) {
      ++count;
    }
  }
  return count;
}

void Recorder::Finalize() {
  for (auto& [_, call] : running_calls_) {
    finalized_calls_.push_back(Lost(call));
  }
}

}  // namespace whirl::histories

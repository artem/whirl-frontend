#pragma once

#include <whirl/matrix/history/history.hpp>

#include <whirl/matrix/common/allocator.hpp>

#include <map>
#include <vector>

namespace whirl::history {

struct RunningCall {
  std::string method;
  Arguments arguments;
  TimePoint start_time;

  MethodCall CompleteWith(Value result) {
    return MethodCall{
        method, arguments, result, start_time, GlobalNow(), true};
  }

  MethodCall NeverComplete() {
    return MethodCall{
        method, arguments, Value::Void(), start_time, 0, false};
  }
};

class Recorder {
 public:
  // Context: Server
  size_t StartCall(const std::string& method, const std::string& input) {
    GlobalHeapScope g;

    size_t id = ++call_id_;
    running_calls_.emplace(id, RunningCall{method, Arguments{input}, GlobalNow()});
    return id;
  }

  // Context: Server
  void CompleteCall(size_t id, const std::string& output) {
    GlobalHeapScope g;

    auto it = running_calls_.find(id);

    auto pending_call = std::move(it->second);
    running_calls_.erase(it);

    completed_calls_.push_back(pending_call.CompleteWith(output));
  }

  void Stop() {
    for (auto& [_, call] : running_calls_) {
      completed_calls_.push_back(call.NeverComplete());
    }
  }

  const History& GetHistory() const {
    return completed_calls_;
  }

 private:
  std::vector<MethodCall> completed_calls_;
  size_t call_id_{0};
  std::map<size_t, RunningCall> running_calls_;
};

}  // namespace whirl::history

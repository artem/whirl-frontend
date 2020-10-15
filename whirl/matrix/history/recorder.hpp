#pragma once

#include <whirl/matrix/history/history.hpp>

#include <map>
#include <vector>

namespace whirl::histories {

struct RunningCall {
  std::string method;
  Arguments arguments;
  TimePoint start_time;

  Call CompleteWith(Value result);
  Call MaybeCompleted();
};

class Recorder {
 public:
  // Context: Server
  size_t CallStarted(const std::string& method, const std::string& input);

  // Context: Server
  void CallCompleted(size_t id, const std::string& output);

  // Context: Server
  void CallMaybeCompleted(size_t id);

  // Context: Server
  void Remove(size_t id);

  // Context: World
  void Stop();

  const History& GetHistory() const {
    return completed_calls_;
  }

 private:
  std::vector<Call> completed_calls_;
  size_t call_id_{0};
  std::map<size_t, RunningCall> running_calls_;
};

}  // namespace whirl::histories

#pragma once

#include <whirl/matrix/history/history.hpp>

#include <map>
#include <vector>

namespace whirl::histories {

class Recorder {
  struct RunningCall {
    std::string method;
    Arguments arguments;
    TimePoint start_time;
    CallLabels labels;
  };

 public:
  using Cookie = size_t;

  // Context: Server
  void AddLabel(Cookie id, const std::string& label);

  // Context: Global
  size_t NumCompletedCalls() const {
    return completed_calls_.size();
  }

  // Context: Server
  Cookie CallStarted(const std::string& method, const std::string& input);

  // Context: Server
  void CallCompleted(Cookie id, const std::string& output);

  // Context: Server
  void CallMaybeCompleted(Cookie id);

  // Context: Server
  void RemoveCall(Cookie id);

  // Context: World
  void Finalize();

  // After Finalize
  const History& GetHistory() const {
    return completed_calls_;
  }

 private:
  static Call Complete(const RunningCall& call, Value output);
  static Call MaybeComplete(const RunningCall& call);

 private:
  std::vector<Call> completed_calls_;
  Cookie next_cookie{0};
  std::map<Cookie, RunningCall> running_calls_;
};

}  // namespace whirl::histories

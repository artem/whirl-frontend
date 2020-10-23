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

  // Context: Global
  size_t NumCompletedCalls() const;

  // Context: Server
  Cookie CallStarted(const std::string& method, const std::string& input);

  // Context: Server
  void AddLabel(Cookie id, const std::string& label);

  // Context: Server
  void CallCompleted(Cookie id, const std::string& output);

  // Context: Server
  void CallLost(Cookie id);

  // Context: Server
  void RemoveCall(Cookie id);

  // Context: World
  void Finalize();

  // After Finalize
  const History& GetHistory() const {
    return finalized_calls_;
  }

 private:
  // Finalizers
  static Call Complete(const RunningCall& call, Value output);
  static Call Lost(const RunningCall& call);

 private:
  std::vector<Call> finalized_calls_;
  Cookie next_id_{0};
  std::map<Cookie, RunningCall> running_calls_;
};

}  // namespace whirl::histories

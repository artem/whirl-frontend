#pragma once

#include <whirl/history/history.hpp>

#include <map>
#include <vector>

namespace whirl::matrix {

class HistoryRecorder {
  struct RunningCall {
    std::string method;
    histories::Arguments arguments;
    TimePoint start_time;
    histories::CallLabels labels;
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
  const histories::History& GetHistory() const {
    return finalized_calls_;
  }

 private:
  // Finalizers
  static histories::Call Complete(const RunningCall& call,
                                  histories::Value output);
  static histories::Call Lost(const RunningCall& call);

 private:
  std::vector<histories::Call> finalized_calls_;
  Cookie next_id_{0};
  std::map<Cookie, RunningCall> running_calls_;
};

}  // namespace whirl::matrix

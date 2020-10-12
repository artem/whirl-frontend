#pragma once

#include <whirl/matrix/history/models/kv.hpp>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

template <typename State>
class LinChecker {
 public:
  LinChecker(History history)
    : calls_(history), count_(history.size()) {
  }

  bool Check() {
    return Search(State());
  }

 private:
  bool StepInto(size_t i, State next);

  bool Search(State s) {
    //PrintSearchState();

    if (count_ == 0) {
      return true;
    }

    for (size_t i = 0; i < count_; ++i) {
      bool candidate = true;

      for (size_t j = 0; j < count_; ++j) {
        if (calls_[j] < calls_[i]) {
          candidate = false;
          break;
        }
      }

      if (!candidate) {
        continue;
      }

      if (!calls_[i].IsCompleted()) {
        if (StepInto(i, s)) {
          return true;
        }
      }

      auto result = s.Apply(calls_[i].method, calls_[i].arguments);

      if (result.IsOk()) {
        if (!calls_[i].IsCompleted() || result.value == calls_[i].result) {
          if (StepInto(i, result.next)) {
            return true;
          }
        }
      }
    }

    return false;
  }

  void PrintSearchState(const State& state) {
    std::cout << "Current progress: " << linear_.size() << " calls" << std::endl;
    for (size_t i = 0; i < linear_.size(); ++i) {
      std::cout << State::Print(linear_[i]) << " -> ";
    }
    std::cout << std::endl;
    std::cout << "State: " << state.Print() << std::endl;
  }

 private:
  std::vector<Call> calls_;
  std::vector<Call> linear_;
  size_t count_;
};

template <typename State>
bool LinChecker<State>::StepInto(size_t i, State next) {
  linear_.push_back(calls_[i]);
  std::swap(calls_[i], calls_[count_ - 1]);
  count_--;

  bool succeeded = Search(std::move(next));

  count_++;
  std::swap(calls_[i], calls_[count_ - 1]);
  linear_.pop_back();

  return succeeded;
}

//////////////////////////////////////////////////////////////////////

template <typename Model>
bool LinCheckBrute(const History& history) {
  LinChecker<Model> checker(history);
  return checker.Check();
}

}  // namespace whirl::history

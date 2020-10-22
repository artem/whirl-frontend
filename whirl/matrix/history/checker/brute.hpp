#pragma once

#include <whirl/matrix/history/history.hpp>
#include <whirl/matrix/history/checker/real_time_order.hpp>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

template <typename Model>
class LinChecker {
  using State = typename Model::State;

 public:
  LinChecker(History history) : calls_(history), count_(history.size()) {
  }

  bool Check() {
    return Search(Model::InitialState());
  }

 private:
  bool StepInto(size_t i, State next_state);

  bool Search(State curr_state) {
    // PrintSearchState(curr_state);

    if (count_ == 0) {
      // linear_ - linearization
      return true;
    }

    for (size_t i = 0; i < count_; ++i) {
      bool candidate = true;

      for (size_t j = 0; j < count_; ++j) {
        if (PrecedesInRealTime(calls_[j], calls_[i])) {
          candidate = false;
          break;
        }
      }

      if (!candidate) {
        continue;
      }

      if (!calls_[i].IsCompleted()) {
        if (StepInto(i, curr_state)) {
          return true;
        }
      }

      auto result = Model::Apply(curr_state, calls_[i].method, calls_[i].arguments);

      if (result.ok) {
        if (!calls_[i].IsCompleted() || result.value == calls_[i].result) {
          if (StepInto(i, std::move(result.next_state))) {
            return true;
          }
        }
      }
    }

    return false;
  }

  void PrintSearchState(const State& state) {
    std::cout << "Current progress: " << linear_.size() << " calls"
              << std::endl;
    for (size_t i = 0; i < linear_.size(); ++i) {
      std::cout << Model::PrintCall(linear_[i]) << " -> ";
    }
    std::cout << std::endl;
    std::cout << "State: " << Model::Print(state) << std::endl;
  }

 private:
  std::vector<Call> calls_;
  std::vector<Call> linear_;
  size_t count_;
};

template <typename State>
bool LinChecker<State>::StepInto(size_t i, State next_state) {
  linear_.push_back(calls_[i]);
  std::swap(calls_[i], calls_[count_ - 1]);
  count_--;

  bool succeeded = Search(std::move(next_state));

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

}  // namespace whirl::histories

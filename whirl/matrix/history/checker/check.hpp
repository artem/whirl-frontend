#pragma once

#include <whirl/matrix/history/history.hpp>
#include <whirl/matrix/history/checker/brute.hpp>

namespace whirl::histories {

template <typename Model>
History Cleanup(const History& history) {
  History result;
  for (auto& call : history) {
    if (!call.IsCompleted() && !Model::IsMutation(call)) {
      // Skip
    } else {
      result.push_back(call);
    }
  }
  return result;
}

template <typename Model>
bool LinCheckImpl(History history) {
  // Limit search iterations
  static const size_t kTimeLimit = 777777;
  return LinCheckBrute<Model>(history, kTimeLimit);
}

template <typename Model>
bool LinCheck(History history) {
  // Cleanup
  history = Cleanup<Model>(history);
  // Decompose to independent histories
  auto sub_histories = Model::Decompose(history);
  // Check
  for (const auto& sub : sub_histories) {
    if (!LinCheckImpl<Model>(sub)) {
      return false;
    }
  }
  return true;
}

}  // namespace whirl::histories

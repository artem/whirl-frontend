#pragma once

#include <whirl/matrix/history/history.hpp>
#include <whirl/matrix/history/checker/brute.hpp>

namespace whirl::histories {

template <typename Model>
History Prepare(const History& history) {
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
bool LinCheck(const History& history) {
  return LinCheckBrute<Model>(
      Prepare<Model>(history));
}

}  // namespace whirl::histories

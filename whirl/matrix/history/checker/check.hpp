#pragma once

#include <whirl/matrix/history/history.hpp>
#include <whirl/matrix/history/checker/brute.hpp>

namespace whirl::histories {

template <typename Model>
bool LinCheck(const History& history) {
  return LinCheckBrute<Model>(history);
}

}  // namespace whirl::histories

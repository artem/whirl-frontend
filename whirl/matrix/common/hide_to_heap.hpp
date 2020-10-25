#pragma once

namespace whirl {

template <typename T>
void HideToHeap(T&& obj) {
  new T(std::move(obj));
}

}  // namespace whirl

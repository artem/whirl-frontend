#pragma once

#include <whirl/matrix/history/all.hpp>

#include <sstream>

namespace whirl::history {

template <typename K, typename V>
void PrintKVHistory(History h) {
  for (auto& call : h) {
    std::stringstream call_out;

    call_out << call.started << " ---> " << call.method;
    if (call.method == "Set") {
      // Arguments
      auto [k, v] = call.arguments.As<K, V>();
      call_out << "(" << k << ", " << v << ")";

      // Return value
      if (call.IsCompleted()) {
        call_out << ": void";
      } else {
        call_out << ": ?";
      }

    } else if (call.method == "Get") {
      // Arguments
      auto [k] = call.arguments.As<K>();
      call_out << "(" << k << ")";

      // Return value
      if (call.IsCompleted()) {
        call_out << ": " << call.result.As<V>();
      } else {
        call_out << ": ?";
      }
    }

    if (call.IsCompleted()) {
      call_out << " ---> " << call.completed;
    }

    std::cout << call_out.str() << std::endl;
  }
}

}  // namespace whirl::history

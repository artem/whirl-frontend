#pragma once

#include <whirl/matrix/history/history.hpp>
#include <whirl/matrix/history/printers/print.hpp>

#include <sstream>

namespace whirl::history {

//////////////////////////////////////////////////////////////////////

template <typename K, typename V>
struct KVCallPrinter {
  static std::string Print(const MethodCall& call) {
    std::stringstream out;

    if (call.method == "Set") {
      PrintSet(call, out);
    } else if (call.method == "Get") {
      PrintGet(call, out);
    }

    return out.str();
  }

  static void PrintSet(const MethodCall& call, std::ostream& out) {
    // Name
    out << call.method;

    // Arguments
    auto [k, v] = call.arguments.As<K, V>();
    out << "(" << k << ", " << v << ")";

    // Return value
    if (call.IsCompleted()) {
      //out << ": void";
    } else {
      out << "?";
    }
  }

  static void PrintGet(const MethodCall& call, std::ostream& out) {
    out << call.method;

    // Arguments
    auto [k] = call.arguments.As<K>();
    out << "(" << k << ")";

    // Return value
    if (call.IsCompleted()) {
      out << ": " << call.result.As<V>();
    } else {
      out << ": ?";
    }
  }
};

//////////////////////////////////////////////////////////////////////

template <typename K, typename V>
void PrintKVHistory(History h) {
  Print<KVCallPrinter<K, V>>(h);
}

}  // namespace whirl::history

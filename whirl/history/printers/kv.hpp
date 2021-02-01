#pragma once

#include <whirl/history/history.hpp>
#include <whirl/history/printers/print.hpp>

#include <sstream>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

template <typename K, typename V>
struct KVCallPrinter {
  static std::string Print(const Call& call) {
    std::stringstream out;

    if (call.method == "Set") {
      PrintSet(call, out);
    } else if (call.method == "Get") {
      PrintGet(call, out);
    } else if (call.method == "Cas") {
      PrintCas(call, out);
    }

    return out.str();
  }

  static void PrintSet(const Call& call, std::ostream& out) {
    // Name
    out << call.method;

    // Arguments
    auto [k, v] = call.arguments.As<K, V>();
    out << "(" << k << ", " << v << ")";

    // Return value
    if (call.IsCompleted()) {
      // out << ": void";
    } else {
      out << "?";
    }
  }

  static void PrintGet(const Call& call, std::ostream& out) {
    out << call.method;

    // Arguments
    auto [k] = call.arguments.As<K>();
    out << "(" << k << ")";

    // Return value
    if (call.IsCompleted()) {
      out << ": " << call.result.As<V>();
    } else {
      out << "?";
    }
  }

  static void PrintCas(const Call& call, std::ostream& out) {
    out << call.method;

    // Arguments
    auto [key, expected_value, target_value] = call.arguments.As<K, V, V>();
    out << "(" << key << ", " << expected_value << ", " << target_value << ")";

    // Return value
    if (call.IsCompleted()) {
      V old_value = call.result.As<V>();
      out << ": " << old_value;
    } else {
      out << "?";
    }
  }
};

//////////////////////////////////////////////////////////////////////

template <typename K, typename V>
void PrintKVHistory(const History& history, std::ostream& out) {
  Print<KVCallPrinter<K, V>>(history, out);
}

}  // namespace whirl::histories

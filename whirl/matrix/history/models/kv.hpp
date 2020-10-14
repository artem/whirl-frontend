#pragma once

#include <whirl/matrix/history/history.hpp>

#include <wheels/support/result.hpp>

#include <map>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

// Defaults

template <typename V>
V KVDefaultValue() {
  return V{};
}

template <>
int KVDefaultValue<int>() {
  return 0;
}

//////////////////////////////////////////////////////////////////////

template <typename K, typename V>
class KVStoreModel {
 public:
  using State = std::map<K, V>;

  struct Result {
    bool ok;
    Value value;
    State next;
  };

  static Result Apply(const State& state, const std::string& method, const Arguments& arguments) {
    if (method == "Set") {
      // Set

      auto [k, v] = arguments.As<K, V>();

      State next = state;
      next.insert_or_assign(k, v);
      return {true, Value::Void(), next};

    } else if (method == "Get") {
      // Get

      auto [k] = arguments.As<K>();

      auto k_it = state.find(k);
      if (k_it == state.end()) {
        return {true, Value::Make(KVDefaultValue<V>()), state};
      } else {
        return {true, Value::Make(k_it->second), state};
      }
    }

    WHEELS_UNREACHABLE();
  }

  static bool IsMutation(const std::string& method) {
    return method == "Set";
  }

  static bool IsReadOnly(const std::string& method) {
    return method == "Get";
  }

  static std::string Print(State state) {
    std::stringstream out;
    out << "{";
    for (auto& [k, v] : state) {
      out << k << " -> " << v << ", ";
    }
    out << "}";
    return out.str();
  }

  static std::string PrintCall(const Call& call) {
    return KVCallPrinter<K,V>::Print(call);
  }
};

}  // namespace whirl::histories

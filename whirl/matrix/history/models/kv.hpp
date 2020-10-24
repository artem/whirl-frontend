#pragma once

#include <whirl/matrix/history/history.hpp>

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

// KV Store sequential specification
// --InitialState-> S_0 --Apply--> S_1 --Apply-> S_2 ...

template <typename K, typename V>
class KVStoreModel {
 public:
  using State = std::map<K, V>;

  static State InitialState() {
    return {};
  }

  struct Result {
    bool ok;
    Value value;
    State next_state;
  };

  static Result Apply(const State& current, const std::string& method,
                      const Arguments& arguments) {
    if (method == "Set") {
      // Set

      auto [k, v] = arguments.As<K, V>();

      State next{current};
      next.insert_or_assign(k, v);
      return {true, Value::Void(), std::move(next)};

    } else if (method == "Get") {
      // Get

      auto [k] = arguments.As<K>();

      auto k_it = current.find(k);
      if (k_it == current.end()) {
        return {true, Value::Make(KVDefaultValue<V>()), current};
      } else {
        return {true, Value::Make(k_it->second), current};
      }
    }

    WHEELS_UNREACHABLE();
  }

  static bool IsMutation(const Call& call) {
    return call.method == "Set";
  }

  static bool IsReadOnly(const Call& call) {
    return call.method == "Get";
  }

  static std::vector<History> Decompose(const History& history) {
    // TODO: split by key
    return {history};
  }

  static std::string Print(State state) {
    std::stringstream out;
    out << "{";
    for (const auto& [k, v] : state) {
      out << k << " -> " << v << ", ";
    }
    out << "}";
    return out.str();
  }

  static std::string PrintCall(const Call& call) {
    return KVCallPrinter<K, V>::Print(call);
  }
};

}  // namespace whirl::histories

#pragma once

#include <whirl/history/history.hpp>

#include <whirl/history/printers/kv.hpp>

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
class KVStoreState {
 public:
  void Set(K k, V v) {
    map_.insert_or_assign(k, v);
  }

  V Get(K k) const {
    auto it = map_.find(k);
    if (it != map_.end()) {
      return it->second;
    }
    return KVDefaultValue<V>();
  }

  V Cas(K k, V expected_v, V target_v) {
    V old = Get(k);
    if (old == expected_v) {
      Set(k, target_v);
    }
    return old;
  }

 private:
  std::map<K, V> map_;
};

//////////////////////////////////////////////////////////////////////

// KV Store sequential specification
// --InitialState-> S_0 --Apply--> S_1 --Apply-> S_2 ...

template <typename K, typename V>
class KVStoreModel {
 public:
  using State = KVStoreState<K, V>;

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
      next.Set(k, v);
      return {true, Value::MakeUnit(), std::move(next)};

    } else if (method == "Get") {
      // Get

      auto [k] = arguments.As<K>();
      V v = current.Get(k);
      return {true, Value::Make(v), current};

    } else if (method == "Cas") {
      // Cas aka Compare-And-Set

      auto [k, expected_v, target_v] = arguments.As<K, V, V>();
      State next{current};
      V old = next.Cas(k, expected_v, target_v);
      return {true, Value::Make(old), std::move(next)};
    }

    WHEELS_UNREACHABLE();
  }

  static bool IsMutation(const Call& call) {
    return call.method == "Set" || call.method == "Cas";
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

  using CallPrinter = KVCallPrinter<K, V>;

  static std::string PrintCall(const Call& call) {
    return CallPrinter::Print(call);
  }
};

}  // namespace whirl::histories

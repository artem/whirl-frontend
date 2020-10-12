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
class KVStoreModel : public std::map<K, V> {
  using Model = KVStoreModel<K, V>;
 public:
  struct Result {
    bool ok;
    Value value;
    Model next;

    bool IsOk() const {
      return ok;
    }
  };

  KVStoreModel() {
    // Nop
  }

  std::string Print() {
    std::stringstream out;
    out << "{";
    for (auto& [k, v] : *this) {
      out << k << " -> " << v << ", ";
    }
    out << "}";
    return out.str();
  }

  static std::string Print(const Call& call) {
    return KVCallPrinter<K,V>::Print(call);
  }

  static bool IsMutation(const std::string& method) {
    return method == "Set";
  }

  static bool IsReadOnly(const std::string& method) {
    return method == "Get";
  }

  Result Apply(const std::string& method, const Arguments& arguments) {
    if (method == "Set") {
      // Set

      auto [k, v] = arguments.As<K, V>();

      Model next = *this;
      next.insert_or_assign(k, v);
      return {true, Value::Void(), next};

    } else if (method == "Get") {
      // Get

      auto [k] = arguments.As<K>();

      auto k_it = this->find(k);
      if (k_it == this->end()) {
        return {true, Value::Make(KVDefaultValue<V>()), *this};
      } else {
        return {true, Value::Make(k_it->second), *this};
      }
    }

    WHEELS_UNREACHABLE();
  }
};

}  // namespace whirl::histories

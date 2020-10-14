#pragma once

namespace whirl {

template <typename K, typename V>
class KVStoreSpec {
  using State = std::map<K, V>;

  struct Call {
    std::string method;
    Arguments arguments;
  };

  struct Result {
    bool ok;
    Result<Value> result;
    State next;
  };

  Apply(const State& state, const Call& call) {

  }


};


}  // namespace whirl

#pragma once

#include <whirl/helpers/serialize.hpp>
#include <whirl/matrix/world/time.hpp>

#include <string>
#include <vector>

namespace whirl::history {

//////////////////////////////////////////////////////////////////////

class Value {
 public:
  Value(std::string bytes)
      : bytes_(std::move(bytes)) {
  }

  static Value Void() {
    return Value{""};
  }

  template<typename T>
  T As() const {
    return Deserialize<T>(bytes_);
  }

 private:
  std::string bytes_;
};

//////////////////////////////////////////////////////////////////////

class Arguments {
 public:
  Arguments(std::string bytes)
      : bytes_(std::move(bytes)) {
  }

  static Arguments Empty() {
    return Arguments{""};
  }

  template<typename ... Args>
  auto As() const {
    return Deserialize < std::tuple < Args...>>(bytes_);
  }

 private:
  std::string bytes_;
};

//////////////////////////////////////////////////////////////////////

struct MethodCall {
  std::string method;
  Arguments arguments;
  Value result;
  TimePoint start_time;
  TimePoint end_time;
  bool is_completed;

  bool IsCompleted() const {
    return is_completed;
  }
};

//////////////////////////////////////////////////////////////////////

using History = std::vector<MethodCall>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::history

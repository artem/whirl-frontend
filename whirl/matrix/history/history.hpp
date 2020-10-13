#pragma once

#include <whirl/helpers/serialize.hpp>
#include <whirl/matrix/world/time.hpp>

#include <string>
#include <vector>
#include <optional>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

class Value {
 public:
  Value(std::string bytes)
      : bytes_(std::move(bytes)) {
  }

  static Value Void() {
    return Value{""};
  }

  template <typename T>
  static Value Make(const T& value) {
    return Value(Serialize<T>(value));
  }

  template<typename T>
  T As() const {
    return Deserialize<T>(bytes_);
  }

  bool operator==(const Value& that) const {
    return bytes_ == that.bytes_;
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

  static Arguments EmptyList() {
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

struct Call {
  std::string method;
  Arguments arguments;
  Value result;
  TimePoint start_time;
  std::optional<TimePoint> end_time;

  bool IsCompleted() const {
    return end_time.has_value();
  }
};

//////////////////////////////////////////////////////////////////////

using History = std::vector<Call>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::histories

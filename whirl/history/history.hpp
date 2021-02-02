#pragma once

#include <whirl/time.hpp>

#include <whirl/cereal/serialize.hpp>
#include <whirl/cereal/tuple.hpp>
#include <whirl/cereal/unit.hpp>

#include <string>
#include <vector>
#include <optional>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

class Value {
 public:
  Value(std::string bytes) : bytes_(std::move(bytes)) {
  }

  // For functions that return void
  static Value MakeUnit() {
    return SerializedUnit();
  }

  template <typename T>
  static Value Make(const T& value) {
    return Value(Serialize<T>(value));
  }

  template <typename T>
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
  Arguments(std::string bytes) : bytes_(std::move(bytes)) {
  }

  // Empty std::tuple
  static Arguments MakeEmpty() {
    return SerializeValues();
  }

  template <typename... Args>
  auto As() const {
    return Deserialize<std::tuple<Args...>>(bytes_);
  }

 private:
  std::string bytes_;
};

//////////////////////////////////////////////////////////////////////

using CallLabels = std::vector<std::string>;

//////////////////////////////////////////////////////////////////////

struct Call {
  std::string method;
  Arguments arguments;
  // std::nullopt - incomplete call,
  // wheels::Unit (std::monostate) - void
  std::optional<Value> result;
  TimePoint start_time;
  std::optional<TimePoint> end_time;
  CallLabels labels;

  bool IsCompleted() const {
    return end_time.has_value();
  }
};

//////////////////////////////////////////////////////////////////////

using History = std::vector<Call>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::histories

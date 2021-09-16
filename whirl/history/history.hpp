#pragma once

#include <whirl/time.hpp>

#include <muesli/serialize.hpp>
#include <muesli/tuple.hpp>

#include <cereal/types/variant.hpp>

#include <wheels/support/unit.hpp>

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
    return muesli::Serialize(wheels::Unit{});
  }

  template <typename T>
  static Value Make(const T& value) {
    return Value(muesli::Serialize<T>(value));
  }

  template <typename T>
  T As() const {
    return muesli::Deserialize<T>(bytes_);
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
    return muesli::SerializeValues();
  }

  template <typename... Args>
  auto As() const {
    return muesli::Deserialize<std::tuple<Args...>>(bytes_);
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

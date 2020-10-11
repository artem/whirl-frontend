#pragma once

#include <whirl/helpers/serialize.hpp>

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/common/allocator.hpp>

#include <whirl/matrix/log/log.hpp>

#include <map>
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

  template <typename T>
  T As() {
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

  template <typename ... Args>
  auto As() {
    return Deserialize<std::tuple<Args...>>(bytes_);
  }

 private:
  std::string bytes_;
};

//////////////////////////////////////////////////////////////////////

struct MethodCall {
  std::string method;
  Arguments arguments;
  Value result;
  TimePoint started;
  TimePoint completed;
  bool is_completed;

  bool IsCompleted() const {
    return is_completed;
  }
};

using History = std::vector<MethodCall>;

//////////////////////////////////////////////////////////////////////

struct StartedCall {
  std::string method;
  Arguments arguments;
  TimePoint started;

  MethodCall CompleteWith(Value result) {
    return MethodCall{
      method, arguments, result, started, GlobalNow(), true};
  }

  MethodCall NeverComplete() {
    return MethodCall{
      method, arguments, Value::Void(), started, 0, false};
  }
};

class HistoryRecorder {
 public:
  // Context: Server
  size_t StartCall(const std::string& method, const std::string& input) {
    GlobalHeapScope g;

    size_t id = ++call_id_;
    started_calls_.emplace(id, StartedCall{method, Arguments{input}, GlobalNow()});
    return id;
  }

  // Context: Server
  void CompleteCall(size_t id, const std::string& output) {
    GlobalHeapScope g;

    auto it = started_calls_.find(id);

    auto started_call = std::move(it->second);
    started_calls_.erase(it);

    WHIRL_FMT_LOG("Call completed: {}", started_call.method);

    calls_.push_back(started_call.CompleteWith(output));
  }

  History Complete() {
    for (auto& [_, started_call] : started_calls_) {
      calls_.push_back(started_call.NeverComplete());
    }
    return calls_;
  }

  const History& GetHistory() const {
    return calls_;
  }

 private:
  std::vector<MethodCall> calls_;

  size_t call_id_{0};
  std::map<size_t, StartedCall> started_calls_;

  Logger logger_{"History"};
};

//////////////////////////////////////////////////////////////////////

struct IModel {
  virtual ~IModel() = default;

  virtual Value Apply(
      const std::string& method,
      Arguments& arguments) = 0;
};

}  // namespace whirl::history

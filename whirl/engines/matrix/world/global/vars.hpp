#pragma once

#include <cstdlib>
#include <string>
#include <any>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// Global user-set variables

namespace detail {

std::any GetGlobal(const std::string& name);
void SetGlobal(const std::string& name, std::any value);

}  // namespace detail

template <typename T>
T GetGlobal(const std::string& name) {
  return std::any_cast<T>(detail::GetGlobal(name));
}

template <typename T>
void SetGlobal(const std::string& name, T value) {
  detail::SetGlobal(name, std::any{std::move(value)});
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <typename T>
class GlobalVar {
 public:
  explicit GlobalVar(const std::string name) : name_(std::move(name)) {
  }

  void Set(T value) {
    SetGlobal(name_, std::move(value));
  }

  T Get() const {
    return ::whirl::matrix::GetGlobal<T>(name_);
  }

 private:
  std::string name_;
};

}  // namespace detail

template <typename T>
auto GlobalVar(const std::string& name) {
  return detail::GlobalVar<T>{name};
}

//////////////////////////////////////////////////////////////////////

namespace detail {

class GlobalCounter : protected GlobalVar<size_t> {
 public:
  explicit GlobalCounter(std::string name) : GlobalVar(name) {
  }

  size_t Add(size_t d) {
    size_t old = Get();
    Set(old + d);
    return old;
  }

  size_t Increment() {
    return Add(1);
  }

  size_t Get() const {
    return GlobalVar::Get();
  }
};

}  // namespace detail

inline auto GlobalCounter(const std::string& name) {
  return detail::GlobalCounter{name};
}

}  // namespace whirl::matrix

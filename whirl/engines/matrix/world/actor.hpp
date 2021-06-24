#pragma once

#include <whirl/time.hpp>

#include <string>

namespace whirl::matrix {

struct IActor {
  virtual ~IActor() = default;

  virtual const std::string& Name() const = 0;

  virtual void Start() = 0;

  virtual bool IsRunnable() const = 0;
  virtual TimePoint NextStepTime() const = 0;
  virtual void Step() = 0;

  virtual void Shutdown() = 0;
};

}  // namespace whirl::matrix

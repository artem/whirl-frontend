#pragma once

#include <memory>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

struct INodeLogger {
  virtual ~INodeLogger() = default;

  virtual void Log(const std::string& message) = 0;
};

using INodeLoggerPtr = std::shared_ptr<INodeLogger>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

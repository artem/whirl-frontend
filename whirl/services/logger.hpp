#pragma once

#include <memory>
#include <string>

namespace whirl {

//////////////////////////////////////////////////////////////////////

enum class NodeLogLevel {
  Debug = 1,
  Info = 2,
  Error = 3,
};

//////////////////////////////////////////////////////////////////////

struct INodeLogger {
  virtual ~INodeLogger() = default;

  virtual void Log(NodeLogLevel level, const std::string& message) = 0;
};

using INodeLoggerPtr = std::shared_ptr<INodeLogger>;

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

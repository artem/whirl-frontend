#pragma once

#include <memory>

namespace whirl {

struct IFileSystem {
  virtual ~IFileSystem() = default;

  // TODO: more useful API
  virtual void RmRf() = 0;
};

using IFileSystemPtr = IFileSystem*;

}  // namespace whirl

#pragma once

#include <whirl/fs/path.hpp>

#include <wheels/support/memspan.hpp>

#include <string>

namespace whirl {

///////////////////////////////////////////////////////////////////////

using Fd = int;

///////////////////////////////////////////////////////////////////////

enum class FileMode {
  Append,  // Do not format
  Read
};

///////////////////////////////////////////////////////////////////////

struct IFileSystem {
  virtual ~IFileSystem() = default;

  virtual bool Exists(const FsPath& file_path) const = 0;

  // FileMode::Append creates file if it does not exist
  virtual Fd Open(const FsPath& file_path, FileMode mode) = 0;

  // Only for FileMode::Append
  // Blocking
  // Atomic?
  virtual void Append(Fd fd, wheels::ConstMemView data) = 0;

  // Only for FileMode::Read
  // Blocking
  virtual size_t Read(Fd fd, wheels::MutableMemView buffer) = 0;

  virtual void Close(Fd fd) = 0;
};

}  // namespace whirl

#pragma once

#include <whirl/node/fs/path.hpp>

#include <wheels/result/result.hpp>
#include <wheels/memory/view.hpp>

#include <string>
#include <vector>

namespace whirl::node::fs {

///////////////////////////////////////////////////////////////////////

using Fd = int;

///////////////////////////////////////////////////////////////////////

enum class FileMode {
  Append,  // Do not format
  Read
};

///////////////////////////////////////////////////////////////////////

// Local file system (or distributed like Colossus?)

struct IFileSystem {
  virtual ~IFileSystem() = default;

  // Metadata

  // Creates new empty file and returns true
  // if file with path `file_path` does not exist yet,
  // returns false otherwise
  virtual wheels::Result<bool> Create(const Path& file_path) = 0;

  // Missing file is Ok
  virtual wheels::Status Delete(const Path& file_path) = 0;

  virtual bool Exists(const Path& file_path) const = 0;

  virtual FileList ListFiles(std::string_view prefix) = 0;

  // Data

  // FileMode::Append creates file if it does not exist
  virtual wheels::Result<Fd> Open(const Path& file_path, FileMode mode) = 0;

  // Only for FileMode::Append
  // Blocking
  // Atomic?
  virtual wheels::Status Append(Fd fd, wheels::ConstMemView data) = 0;

  // Only for FileMode::Read
  // Blocking
  virtual wheels::Result<size_t> Read(Fd fd, wheels::MutableMemView buffer) = 0;

  virtual wheels::Status Close(Fd fd) = 0;

  // TODO: Fsync

  // Paths

  virtual Path RootPath() const = 0;

  virtual Path TmpPath() const = 0;

  // Appends `name` component to path `base`
  virtual std::string PathAppend(const std::string& base,
                                 const std::string& name) const = 0;
};

}  // namespace whirl::node::fs

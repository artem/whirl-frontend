#pragma once

#include <whirl/node/fs/path.hpp>

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
  virtual bool Create(const Path& file_path) = 0;

  // Missing file is Ok
  virtual void Delete(const Path& file_path) = 0;

  virtual bool Exists(const Path& file_path) const = 0;

  virtual std::vector<std::string> ListFiles(std::string_view prefix) = 0;

  // Data

  // FileMode::Append creates file if it does not exist
  virtual Fd Open(const Path& file_path, FileMode mode) = 0;

  // Only for FileMode::Append
  // Blocking
  // Atomic?
  virtual void Append(Fd fd, wheels::ConstMemView data) = 0;

  // Only for FileMode::Read
  // Blocking
  virtual size_t Read(Fd fd, wheels::MutableMemView buffer) = 0;

  virtual void Close(Fd fd) = 0;
};

}  // namespace whirl::node::fs

#pragma once

#include <whirl/fs/fs.hpp>

#include <whirl/engines/matrix/fs/file.hpp>

#include <whirl/logger/log.hpp>

#include <map>
#include <string>
#include <memory>

namespace whirl::matrix::fs {

//////////////////////////////////////////////////////////////////////

class FileSystem {
  using FileRef = std::shared_ptr<File>;

  struct OpenedFile {
    node::fs::Fd fd;
    node::fs::Path path;
    node::fs::FileMode mode;
    size_t offset;
    FileRef file;
  };

 public:
  FileSystem() = default;

  // System calls

  bool Exists(const node::fs::Path& file_path) const;

  node::fs::Fd Open(
      const node::fs::Path& file_path,
      node::fs::FileMode mode);

  size_t Read(node::fs::Fd fd, wheels::MutableMemView buffer);
  void Append(node::fs::Fd fd, wheels::ConstMemView data);

  void Close(node::fs::Fd fd);

  void Delete(const node::fs::Path& file_path);

  // Simulation

  // On crash
  void Reset();

  size_t ComputeDigest() const;

 private:
  FileRef FindOrCreateFile(
      const node::fs::Path& file_path,
      node::fs::FileMode open_mode);

  static FileRef CreateFile();

  size_t InitOffset(FileRef f, node::fs::FileMode open_mode);

  void CheckMode(OpenedFile& of, node::fs::FileMode expected);

  OpenedFile& GetOpenedFile(node::fs::Fd fd);

  [[noreturn]] void RaiseError(const std::string& message);

 private:
  // Persistent state
  std::map<node::fs::Path, FileRef> files_;

  // Process (volatile) state
  std::map<node::fs::Fd, OpenedFile> opened_files_;
  node::fs::Fd next_fd_{0};

  Logger logger_{"Filesystem"};
};

}  // namespace whirl::matrix::fs

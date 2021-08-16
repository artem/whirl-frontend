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
    Fd fd;
    FsPath path;
    FileMode mode;
    size_t offset;
    FileRef file;
  };

 public:
  FileSystem() = default;

  // System calls

  bool Exists(const FsPath& file_path) const;
  Fd Open(const FsPath& file_path, FileMode mode);
  size_t Read(Fd fd, wheels::MutableMemView buffer);
  void Append(Fd fd, wheels::ConstMemView data);
  void Close(Fd fd);

  // Simulation

  // On crash
  void Reset();

  size_t ComputeDigest() const;

 private:
  FileRef FindOrCreateFile(const FsPath& file_path, FileMode open_mode);
  static FileRef CreateFile();

  size_t InitOffset(FileRef f, FileMode open_mode);

  void CheckMode(OpenedFile& of, FileMode expected);

  OpenedFile& GetOpenedFile(Fd fd);

  [[noreturn]] void RaiseError(const std::string& message);

 private:
  // Persistent state
  std::map<FsPath, FileRef> files_;

  // Process (volatile) state
  std::map<Fd, OpenedFile> opened_files_;
  Fd next_fd_{0};

  Logger logger_{"Filesystem"};
};

}  // namespace whirl::matrix::fs

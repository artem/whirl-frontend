#pragma once

#include <whirl/node/fs/fs.hpp>

#include <whirl/engines/matrix/fs/file.hpp>
#include <whirl/engines/matrix/fs/path.hpp>

#include <timber/logger.hpp>

#include <wheels/result/result.hpp>

#include <map>
#include <string>
#include <memory>
#include <vector>

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

  using Files = std::map<Path, FileRef>;

  class DirIterator {
   public:
    DirIterator(Files& files) : it_(files.begin()), end_(files.end()) {
    }

    const Path& operator*() {
      return it_->first;
    }

    bool IsValid() const {
      return it_ != end_;
    }

    void operator++() {
      ++it_;
    }

   private:
    Files::const_iterator it_;
    Files::const_iterator end_;
  };

 public:
  FileSystem();

  // System calls
  // Context: Server

  // Metadata

  wheels::Result<bool> Create(const node::fs::Path& file_path);
  wheels::Status Delete(const node::fs::Path& file_path);
  bool Exists(const node::fs::Path& file_path) const;

  DirIterator ListAllFiles();

  // Data

  wheels::Result<node::fs::Fd> Open(const node::fs::Path& file_path, node::fs::FileMode mode);

  wheels::Result<size_t> Read(node::fs::Fd fd, wheels::MutableMemView buffer);
  wheels::Status Append(node::fs::Fd fd, wheels::ConstMemView data);

  wheels::Status Close(node::fs::Fd fd);

  // Paths

  std::string JoinPath(const std::string& base_path,
                       const std::string& name) const;

  // Simulation

  // Fault injection
  void Corrupt(const node::fs::Path& file_path);

  // On crash
  void Reset();

  size_t ComputeDigest() const;

 private:
  FileRef FindOrCreateFile(const node::fs::Path& file_path,
                           node::fs::FileMode open_mode);

  static FileRef CreateFile();

  size_t InitOffset(FileRef f, node::fs::FileMode open_mode);

  void CheckMode(OpenedFile& of, node::fs::FileMode expected);

  OpenedFile& GetOpenedFile(node::fs::Fd fd);

  [[noreturn]] void RaiseError(const std::string& message);

 private:
  // Persistent state
  Files files_;

  // Process (volatile) state
  std::map<node::fs::Fd, OpenedFile> opened_files_;
  node::fs::Fd next_fd_{0};

  timber::Logger logger_;
};

}  // namespace whirl::matrix::fs

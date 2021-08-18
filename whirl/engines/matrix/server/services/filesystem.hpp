#pragma once

#include <whirl/fs/fs.hpp>
#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/server/services/detail/disk.hpp>
#include <whirl/engines/matrix/fs/fs.hpp>

namespace whirl::matrix {

class FS: public node::fs::IFileSystem {
 public:
  FS(matrix::fs::FileSystem* impl, ITimeService* time_service)
    : disk_(time_service), impl_(impl) {
  }

  void Create(const node::fs::Path& file_path) override {
    impl_->Create(file_path);
  }

  void Delete(const node::fs::Path& file_path) override {
    impl_->Delete(file_path);
  }

  bool Exists(const node::fs::Path& file_path) const override {
    return impl_->Exists(file_path);
  }

  std::vector<std::string> ListFiles(std::string_view prefix) override {
    // All allocations are made in "userspace"
    std::vector<std::string> listed;

    auto iter = impl_->ListAllFiles();
    while (iter.IsValid()) {
      if ((*iter).starts_with(prefix)) {
        listed.push_back(*iter);
      }
      ++iter;
    }

    return listed;
  }

  // FileMode::Append creates file if it does not exist
  node::fs::Fd Open(const node::fs::Path& file_path, node::fs::FileMode mode) override {
    return impl_->Open(file_path, mode);
  }

  // Only for FileMode::Append
  void Append(node::fs::Fd fd, wheels::ConstMemView data) override {
    disk_.Write(data.Size());
    impl_->Append(fd, data);
  }

  // Only for FileMode::Read
  size_t Read(node::fs::Fd fd, wheels::MutableMemView buffer) override {
    disk_.Read(buffer.Size());  // Blocks
    return impl_->Read(fd, buffer);
  }

  void Close(node::fs::Fd fd) override {
    impl_->Close(fd);
  }

 private:
  // Emulate latency
  matrix::detail::Disk disk_;

  matrix::fs::FileSystem* impl_;
};

}  // namespace whirl::matrix

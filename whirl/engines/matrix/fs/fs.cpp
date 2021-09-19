#include <whirl/engines/matrix/fs/fs.hpp>

#include <whirl/engines/matrix/helpers/digest.hpp>
#include <whirl/engines/matrix/new/new.hpp>

#include <whirl/engines/matrix/world/global/log.hpp>

#include <timber/log.hpp>

#include <wheels/result/result.hpp>
#include <wheels/result/make.hpp>

using whirl::node::fs::Fd;
using whirl::node::fs::FileMode;

using wheels::Result;
using wheels::Status;

namespace result = wheels::make_result;

namespace whirl::matrix::fs {

FileSystem::FileSystem() : logger_("Filesystem", GetLogBackend()) {
}

// System calls

bool FileSystem::Exists(const node::fs::Path& file_path) const {
  // No allocations here!

  return files_.find(file_path.Repr()) != files_.end();
}

Result<Fd> FileSystem::Open(const node::fs::Path& file_path, FileMode mode) {
  GlobalAllocatorGuard g;

  auto file = FindOrCreateFile(file_path, mode);
  Fd fd = ++next_fd_;
  size_t offset = InitOffset(file, mode);
  opened_files_.emplace(fd, OpenedFile{fd, file_path, mode, offset, file});
  return result::Ok(fd);
}

Result<size_t> FileSystem::Read(Fd fd, wheels::MutableMemView buffer) {
  GlobalAllocatorGuard g;

  OpenedFile& of = GetOpenedFile(fd);
  CheckMode(of, FileMode::Read);

  LOG_DEBUG("Read {} bytes from '{}'", buffer.Size(), of.path);
  size_t bytes_read = of.file->PRead(of.offset, buffer);
  of.offset += bytes_read;
  return result::Ok(bytes_read);
}

Status FileSystem::Append(Fd fd, wheels::ConstMemView data) {
  GlobalAllocatorGuard g;

  OpenedFile& of = GetOpenedFile(fd);
  CheckMode(of, FileMode::Append);
  LOG_DEBUG("Append {} bytes to '{}'", data.Size(), of.path);
  of.file->Append(data);
  return result::Ok();
}

Status FileSystem::Close(Fd fd) {
  GlobalAllocatorGuard g;

  auto it = opened_files_.find(fd);
  if (it != opened_files_.end()) {
    opened_files_.erase(it);
    return result::Ok();
  } else {
    RaiseError("File not found by fd");
  }
}

Result<bool> FileSystem::Create(const node::fs::Path& file_path) {
  GlobalAllocatorGuard g;

  if (files_.contains(file_path.Repr())) {
    return result::Ok(false);
  }

  LOG_INFO("Create new file '{}'", file_path);
  auto f = CreateFile();
  files_.insert({file_path.Repr(), f});
  return result::Ok(true);
}

Status FileSystem::Delete(const node::fs::Path& file_path) {
  GlobalAllocatorGuard g;

  files_.erase(file_path.Repr());
  return result::Ok();
}

FileSystem::DirIterator FileSystem::ListAllFiles() {
  // No allocations here!
  return {files_};
}

std::string FileSystem::RootPath() const {
  // Allocate new string in userspace
  return "/";
}

std::string FileSystem::PathAppend(const std::string& base_path, const std::string& name) const {
  // Allocate new string in userspace

  if (base_path.ends_with('/')) {
    return base_path + name;
  } else {
    return base_path + "/" + name;
  }
}

FileSystem::FileRef FileSystem::FindOrCreateFile(const node::fs::Path& file_path,
                                                 FileMode open_mode) {
  auto it = files_.find(file_path.Repr());

  if (it != files_.end()) {
    return it->second;
  } else {
    // File does not exist
    if (open_mode == FileMode::Append) {
      LOG_INFO("Create file '{}'", file_path);
      auto f = CreateFile();
      files_.insert({file_path.Repr(), f});
      return f;
    } else {
      RaiseError("File not found");
    }
  }
}

FileSystem::FileRef FileSystem::CreateFile() {
  return std::make_shared<File>();
}

void FileSystem::CheckMode(OpenedFile& of, FileMode expected) {
  if (of.mode != expected) {
    RaiseError("Unexpected file mode");
  }
}

size_t FileSystem::InitOffset(FileRef f, FileMode open_mode) {
  switch (open_mode) {
    case FileMode::Read: return 0;
    case FileMode::Append: return f->Size();
  }
}

FileSystem::OpenedFile& FileSystem::GetOpenedFile(Fd fd) {
  auto it = opened_files_.find(fd);
  if (it == opened_files_.end()) {
    RaiseError("Fd not found");
  }
  return it->second;
}

void FileSystem::RaiseError(const std::string& message) {
  WHEELS_PANIC("Filesystem error: " << message);
}

// Simulation

void FileSystem::Corrupt(const node::fs::Path& file_path) {
  WHEELS_UNUSED(file_path);
  // TODO
}

void FileSystem::Reset() {
  // Reset volatile state
  opened_files_.clear();
  next_fd_ = 0;
}

size_t FileSystem::ComputeDigest() const {
  DigestCalculator digest;

  for (const auto& [path, file] : files_) {
    digest.Eat(path);
    digest.Combine(file->ComputeDigest());
  }
  return digest.GetValue();
}

}  // namespace whirl::matrix::fs

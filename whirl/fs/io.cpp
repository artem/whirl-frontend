#include <whirl/fs/io.hpp>

#include <whirl/runtime/runtime.hpp>

namespace whirl::node::fs {

//////////////////////////////////////////////////////////////////////

FileReader::FileReader(IFileSystem* fs, Path file_path) : fs_(fs) {
  fd_ = Fs()->Open(file_path, FileMode::Read);
}

FileReader::FileReader(Path file_path)
    : FileReader(GetRuntime().FileSystem(), file_path) {
}

FileReader::~FileReader() {
  fs_->Close(fd_);
}

size_t FileReader::ReadSome(wheels::MutableMemView buffer) {
  return fs_->Read(fd_, buffer);
}

IFileSystem* FileReader::Fs() {
  return fs_;
}

//////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(IFileSystem* fs, Path file_path) : fs_(fs) {
  fd_ = Fs()->Open(file_path, FileMode::Append);
}

FileWriter::FileWriter(Path file_path)
    : FileWriter(GetRuntime().FileSystem(), file_path) {
}

FileWriter::~FileWriter() {
  Fs()->Close(fd_);
}

void FileWriter::Write(wheels::ConstMemView data) {
  Fs()->Append(fd_, data);
}

void FileWriter::Flush() {
  // Nothing to do
}

IFileSystem* FileWriter::Fs() {
  return fs_;
}

}  // namespace whirl::node::fs

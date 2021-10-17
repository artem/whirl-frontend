#include <whirl/node/fs/io.hpp>

#include <whirl/runtime/access.hpp>

#include <wheels/result/make.hpp>

namespace whirl::node::fs {

//////////////////////////////////////////////////////////////////////

FileReader::FileReader(IFileSystem* fs, Path file_path) : fs_(fs) {
  fd_ = Fs()->Open(file_path, FileMode::Read);
}

FileReader::FileReader(Path file_path)
    : FileReader(GetRuntime().FileSystem(), file_path) {
}

FileReader::~FileReader() {
  fs_->Close(fd_).ExpectOk("Failed to close file");
}

wheels::Result<size_t> FileReader::ReadSome(wheels::MutableMemView buffer) {
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
  Fs()->Close(fd_).ExpectOk("Failed to close file");
}

wheels::Status FileWriter::Write(wheels::ConstMemView data) {
  return Fs()->Append(fd_, data);
}

wheels::Status FileWriter::Flush() {
  // Nothing to do
  return wheels::make_result::Ok();
}

IFileSystem* FileWriter::Fs() {
  return fs_;
}

}  // namespace whirl::node::fs

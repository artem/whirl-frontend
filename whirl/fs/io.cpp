#include <whirl/fs/io.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

FileReader::FileReader(IFileSystem* fs, FsPath file_path)
  : fs_(fs) {
  fd_ = Fs()->Open(file_path, FileMode::Read);
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

FileWriter::FileWriter(IFileSystem* fs, FsPath file_path)
    : fs_(fs) {
  fd_ = Fs()->Open(file_path, FileMode::Append);
}

FileWriter::~FileWriter() {
  Fs()->Close(fd_);
}

size_t FileWriter::Write(wheels::ConstMemView data) {
  Fs()->Append(fd_, data);
  return data.Size();
}

IFileSystem* FileWriter::Fs() {
  return fs_;
}

}  // namespace whirl

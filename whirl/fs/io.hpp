#pragma once

#include <whirl/fs/fs.hpp>

#include <whirl/io/reader.hpp>
#include <whirl/io/writer.hpp>

#include <wheels/support/noncopyable.hpp>

namespace whirl::node::fs {

//////////////////////////////////////////////////////////////////////

class FileReader : public IReader, private wheels::NonCopyable {
 public:
  FileReader(IFileSystem* fs, Path file_path);
  ~FileReader();

  size_t ReadSome(wheels::MutableMemView buffer) override;

 private:
  IFileSystem* Fs();

 private:
  IFileSystem* fs_;
  Fd fd_;
};

//////////////////////////////////////////////////////////////////////

class FileWriter : public IWriter, private wheels::NonCopyable {
 public:
  FileWriter(IFileSystem* fs, Path file_path);
  ~FileWriter();

  size_t Write(wheels::ConstMemView data) override;

 private:
  IFileSystem* Fs();

 private:
  IFileSystem* fs_;
  Fd fd_;
};

}  // namespace whirl::node::fs

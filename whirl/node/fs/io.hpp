#pragma once

#include <whirl/node/fs/filesystem.hpp>

#include <wheels/io/reader.hpp>
#include <wheels/io/writer.hpp>

#include <wheels/support/noncopyable.hpp>

namespace whirl::node::fs {

//////////////////////////////////////////////////////////////////////

class FileReader : public wheels::io::IReader, private wheels::NonCopyable {
 public:
  FileReader(IFileSystem* fs, Path file_path);

  // Access fs via node runtime
  explicit FileReader(Path file_path);

  ~FileReader();

  size_t ReadSome(wheels::MutableMemView buffer) override;

 private:
  IFileSystem* Fs();

 private:
  IFileSystem* fs_;
  Fd fd_;
};

//////////////////////////////////////////////////////////////////////

class FileWriter : public wheels::io::IWriter, private wheels::NonCopyable {
 public:
  FileWriter(IFileSystem* fs, Path file_path);

  // Access fs via runtime
  explicit FileWriter(Path file_path);

  ~FileWriter();

  void Write(wheels::ConstMemView data) override;

  void Flush() override;

 private:
  IFileSystem* Fs();

 private:
  IFileSystem* fs_;
  Fd fd_;
};

}  // namespace whirl::node::fs

#pragma once

#include <whirl/db/write_batch.hpp>

#include <whirl/fs/fs.hpp>
#include <whirl/fs/io.hpp>

#include <whirl/engines/matrix/db/detail/framed.hpp>

#include <whirl/cereal/serialize.hpp>

#include <wheels/io/buffered.hpp>

#include <cereal/types/vector.hpp>

namespace whirl::matrix::db {

// Write ahead log (WAL) writer / reader

struct WALEntry {
  std::vector<node::db::Mutation> muts;

  WHIRL_SERIALIZABLE(muts);
};

//////////////////////////////////////////////////////////////////////

class WALWriter {
 public:
  WALWriter(node::fs::IFileSystem* fs, node::fs::Path file_path)
      : file_writer_(fs, file_path), framed_writer_(&file_writer_) {
  }

  void Append(node::db::WriteBatch batch) {
    AppendImpl({batch.muts});
  }

 private:
  // Atomic
  void AppendImpl(WALEntry entry) {
    framed_writer_.WriteFrame(Serialize(entry));
  }

 private:
  node::fs::FileWriter file_writer_;
  FramedWriter framed_writer_;
};

//////////////////////////////////////////////////////////////////////

class WALReader {
 public:
  WALReader(node::fs::IFileSystem *fs, node::fs::Path log_file_path)
      : file_reader_(fs, log_file_path),
        buf_file_reader_(&file_reader_),
        framed_reader_(&buf_file_reader_) {
  }

  std::optional<node::db::WriteBatch> ReadNext();

 private:
  node::fs::FileReader file_reader_;
  wheels::io::BufferedReader buf_file_reader_;
  FramedReader framed_reader_;
};

}  // namespace whirl::matrix::db

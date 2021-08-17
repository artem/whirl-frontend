#include <whirl/engines/matrix/db/wal.hpp>

using whirl::node::db::WriteBatch;

namespace whirl::matrix::db {

std::optional<WriteBatch> WALReader::ReadNext() {
  auto frame = framed_reader_.ReadNextFrame();
  if (!frame.has_value()) {
    return std::nullopt;
  }
  auto entry = Deserialize<WALEntry>(*frame);
  return node::db::WriteBatch{entry.muts};
}

}  // namespace whirl::matrix::db

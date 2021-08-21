#include <whirl/engines/matrix/db/database.hpp>

#include <whirl/engines/matrix/world/global/random.hpp>

using whirl::node::db::Key;
using whirl::node::db::Value;
using whirl::node::db::WriteBatch;

namespace whirl::matrix::db {

Database::Database(node::fs::IFileSystem* fs) : fs_(fs) {
}

void Database::Open(const std::string& directory) {
  wal_path_ = directory + "/wal";
  wal_.emplace(fs_, wal_path_);
  ReplayWAL();
}

void Database::Put(const Key& key, const Value& value) {
  WHIRL_LOG_INFO("Put('{}', '{}')", key, value);

  node::db::WriteBatch batch;
  batch.Put(key, value);
  DoWrite(batch);
}

void Database::Delete(const Key& key) {
  WHIRL_LOG_INFO("Delete('{}')", key);

  node::db::WriteBatch batch;
  batch.Delete(key);
  Write(batch);
}

std::optional<Value> Database::TryGet(const Key& key) const {
  if (ReadCacheMiss()) {
    // TODO
    // disk_.Read(1);  // Access SSTable-s
  }
  WHIRL_LOG_INFO("TryGet({})", key);
  return mem_table_.TryGet(key);
}

void Database::Write(WriteBatch batch) {
  WHIRL_LOG_INFO("Write({} mutations)", batch.muts.size());
  DoWrite(batch);
}

void Database::DoWrite(WriteBatch& batch) {
  auto guard = write_mutex_.Guard();

  wal_->Append(batch);
  ApplyToMemTable(batch);
}

void Database::ApplyToMemTable(const node::db::WriteBatch& batch) {
  for (const auto& mut : batch.muts) {
    switch (mut.type) {
      case node::db::MutationType::Put:
        WHIRL_LOG_INFO("Put('{}', '{}')", mut.key, *mut.value);
        mem_table_.Put(mut.key, *mut.value);
        break;
      case node::db::MutationType::Delete:
        WHIRL_LOG_INFO("Delete('{}')", mut.key);
        mem_table_.Delete(mut.key);
        break;
    }
  }
}

bool Database::ReadCacheMiss() const {
  // return GlobalRandomNumber(10) == 1;  // Move to time model?
  return false;
}

void Database::ReplayWAL() {
  mem_table_.Clear();

  WHIRL_LOG_INFO("Replaying WAL");

  if (!fs_->Exists(wal_path_)) {
    return;
  }

  WALReader wal_reader(fs_, wal_path_);

  while (auto batch = wal_reader.ReadNext()) {
    ApplyToMemTable(*batch);
  }

  WHIRL_LOG_INFO("Mem table populated");
}

}  // namespace whirl::matrix::db

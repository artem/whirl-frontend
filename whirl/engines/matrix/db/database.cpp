#include <whirl/engines/matrix/db/database.hpp>

#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix::db {

Database::Database(IFileSystem* fs)
: fs_(fs) {
}

void Database::Open(const std::string& directory) {
  wal_path_ = directory + "/wal";
  wal_.emplace(fs_, wal_path_);
  ReplayWAL();
}

void Database::Put(const DbKey& key, const DbValue& value) {
  auto guard = write_mutex_.Guard();

  WHIRL_LOG_INFO("Put({}, {})", key, value);

  wal_->Put(key, value);
  mem_table_.Put(key, value);
}

void Database::Delete(const DbKey& key) {
  auto guard = write_mutex_.Guard();

  WHIRL_LOG_INFO("Delete({})", key);

  wal_->Delete(key);
  mem_table_.Delete(key);
}

std::optional<DbValue> Database::TryGet(const DbKey& key) const {
  if (ReadCacheMiss()) {
    // TODO
    // disk_.Read(1);  // Access SSTable-s
  }
  WHIRL_LOG_INFO("TryGet({})", key);
  return mem_table_.TryGet(key);
}

bool Database::ReadCacheMiss() const {
  //return GlobalRandomNumber(10) == 1;  // Move to time model?
  return false;
}

void Database::ReplayWAL() {
  mem_table_.Clear();

  WHIRL_LOG_INFO("Replaying WAL");

  if (!fs_->Exists(wal_path_)) {
    return;
  }

  WALReader wal_reader(fs_, wal_path_);

  while (auto mut = wal_reader.Next()) {
    switch (mut->type) {
      case MutationType::Put:
        WHIRL_LOG_INFO("Replay Put({}, {})", mut->key, *(mut->value));
        mem_table_.Put(mut->key, *(mut->value));
        break;
      case MutationType::Delete:
        WHIRL_LOG_INFO("Replay Delete({})", mut->key);
        mem_table_.Delete(mut->key);
        break;
    }
  }

  WHIRL_LOG_INFO("Mem table populated");
}

}  // namespace whirl::matrix::db

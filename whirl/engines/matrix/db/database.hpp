#pragma once

#include <whirl/db/database.hpp>
#include <whirl/db/mutation.hpp>

#include <whirl/fs/fs.hpp>

#include <whirl/engines/matrix/db/mem_table.hpp>
#include <whirl/engines/matrix/db/wal.hpp>

#include <whirl/logger/log.hpp>

#include <await/fibers/sync/mutex.hpp>

namespace whirl::matrix::db {

// Implemented in userspace

class Database : public node::db::IDatabase {
 public:
  Database(node::fs::IFileSystem* fs);

  void Open(const std::string& directory) override;

  void Put(const node::db::Key& key,
           const node::db::Value& value) override;

  void Delete(const node::db::Key& key) override;

  std::optional<node::db::Value> TryGet(
      const node::db::Key& key) const override;

 private:
  void ReplayWAL();

  bool ReadCacheMiss() const;

 private:
  node::fs::IFileSystem* fs_;

  std::string wal_path_;

  MemTable mem_table_;
  std::optional<WALWriter> wal_;
  await::fibers::Mutex write_mutex_;

  mutable Logger logger_{"Database"};
};

}  // namespace whirl::matrix::db

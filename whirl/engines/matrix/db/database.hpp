#pragma once

#include <whirl/db/database.hpp>

#include <whirl/engines/matrix/db/mutation.hpp>
#include <whirl/engines/matrix/db/mem_table.hpp>
#include <whirl/engines/matrix/db/wal.hpp>

#include <whirl/logger/log.hpp>

#include <await/fibers/sync/mutex.hpp>

namespace whirl::matrix::db {

// Implemented in userspace

class Database : public IDatabase {
 public:
  Database(IFileSystem* fs);

  void Open(const std::string& directory) override;

  void Put(const DbKey& key, const DbValue& value) override;
  void Delete(const DbKey& key) override;
  std::optional<DbValue> TryGet(const DbKey& key) const override;

 private:
  void ReplayWAL();

  bool ReadCacheMiss() const;

 private:
  IFileSystem* fs_;

  std::string wal_path_;

  MemTable mem_table_;
  std::optional<WALWriter> wal_;
  await::fibers::Mutex write_mutex_;

  mutable Logger logger_{"Database"};
};

}  // namespace whirl::matrix::db

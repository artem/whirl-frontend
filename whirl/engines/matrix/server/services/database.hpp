#pragma once

#include <whirl/services/database.hpp>

#include <whirl/engines/matrix/server/database.hpp>
#include <whirl/engines/matrix/server/services/detail/disk.hpp>
#include <whirl/engines/matrix/world/global/random.hpp>

#include <whirl/logger/log.hpp>

#include <await/futures/util/await.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class DatabaseProxy : public IDatabase {
 public:
  DatabaseProxy(Database& impl, ITimeServicePtr time_service)
      : impl_(impl), disk_(std::move(time_service)) {
  }

  void Put(const std::string& key, const Bytes& value) override {
    DiskWrite();
    impl_.Put(key, value);
  }

  std::optional<Bytes> TryGet(const std::string& key) const override {
    if (ReadCacheMiss()) {
      DiskRead();
    }
    return impl_.TryGet(key);
  }

  void Delete(const std::string& key) override {
    DiskWrite();
    impl_.Delete(key);
  }

 private:
  bool ReadCacheMiss() const {
    return GlobalRandomNumber(10) == 1;
  }

  void DiskRead() const {
    WHIRL_LOG_INFO("Cache miss, read key from disk");
    await::futures::Await(disk_.Read()).ExpectOk();
  }

  void DiskWrite() {
    WHIRL_LOG_INFO("Write key to disk");
    await::futures::Await(disk_.Write()).ExpectOk();
  }

 private:
  Database& impl_;
  detail::Disk disk_;
  mutable Logger logger_{"Database"};
};

}  // namespace whirl::matrix

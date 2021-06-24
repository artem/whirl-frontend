#pragma once

#include <whirl/services/database.hpp>

#include <whirl/engines/matrix/server/storage.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class Database : public IDatabase {
 public:
  Database(PersistentStorage& impl) : impl_(impl) {
  }

  void Put(const std::string& key, const Bytes& value) override {
    impl_.Put(key, value);
  }

  std::optional<Bytes> TryGet(const std::string& key) const override {
    return impl_.TryGet(key);
  }

  void Delete(const std::string& key) override {
    impl_.Delete(key);
  }

 private:
  PersistentStorage& impl_;
};

}  // namespace whirl::matrix

#pragma once

#include <whirl/services/database.hpp>

#include <whirl/engines/matrix/server/database.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class DatabaseProxy : public IDatabase {
 public:
  DatabaseProxy(Database& impl) : impl_(impl) {
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
  Database& impl_;
};

}  // namespace whirl::matrix

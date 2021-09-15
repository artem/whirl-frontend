#pragma once

#include <whirl/node/misc/config.hpp>

namespace whirl::matrix {

class Config : public IConfig {
 public:
  Config(size_t id, std::string pool) : id_(id), pool_(pool) {
  }

  size_t Id() const override {
    return id_;
  }

  const std::string& PoolName() const override {
    return pool_;
  }

  std::optional<std::string> Get(const std::string& key) override {
    WHEELS_UNUSED(key);
    return std::nullopt;
  };

 private:
  size_t id_;
  std::string pool_;
};

}  // namespace whirl::matrix

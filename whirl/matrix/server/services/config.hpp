#pragma once

#include <whirl/services/config.hpp>

namespace whirl {

class Config : public IConfig {
 public:
  Config(size_t id) : id_(id) {
  }

  size_t Id() const override {
    return id_;
  }

  std::string RpcPort() const override {
    return "rpc";
  }

 private:
  size_t id_;
};

}  // namespace whirl

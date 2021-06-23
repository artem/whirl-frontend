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

 private:
  size_t id_;
};

}  // namespace whirl

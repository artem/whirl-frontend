#pragma once

#include <whirl/services/uid.hpp>

#include <whirl/matrix/world/global/random.hpp>
#include <whirl/matrix/world/global/time.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl {

struct UidGenerator : public IUidGenerator {
  UidGenerator(size_t id) : id_(id) {
  }

  Uid Generate() override {
    return wheels::StringBuilder() << GlobalRandomNumber() << "-" << id_ << "-"
                                   << GlobalNow() << "-" << ++request_;
  }

 private:
  const size_t id_;
  size_t request_{0};
};

}  // namespace whirl

#pragma once

#include <whirl/services/uid.hpp>

#include <whirl/matrix/global.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl {

struct UidGenerator : public IUidGenerator {
  Uid Generate() {
    static size_t request = 0;
    return wheels::StringBuilder() << GlobalNow() << "-" << ++request;
  }
};

}  // namespace whirl

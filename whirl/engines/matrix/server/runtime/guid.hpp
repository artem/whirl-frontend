#pragma once

#include <whirl/node/guid/guid.hpp>

#include <whirl/engines/matrix/world/global/random.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/guids.hpp>

#include <wheels/support/string_builder.hpp>

namespace whirl::matrix {

struct GuidGenerator : public node::IGuidGenerator {
  GuidGenerator(size_t server_id) : server_id_(server_id) {
  }

  node::Guid Generate() override {
    // return GenerateLocal();

    // Short and globally unique id
    return GenerateGuid();
  }

 private:
  // {random-number}-{server-id}-{global-time}-{uid-request}
  node::Guid GenerateLocal() {
    return wheels::StringBuilder() << GlobalRandomNumber() << "-" << server_id_
                                   << "-" << GlobalNow() << "-" << ++request_;
  }

 private:
  const size_t server_id_;
  size_t request_{0};
};

}  // namespace whirl::matrix

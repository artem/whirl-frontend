#pragma once

namespace whirl {

struct EmptyMessage {
  template <typename Archive>
  void serialize(Archive& /*a*/) {
  }
};

}  // namespace whirl

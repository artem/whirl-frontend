#pragma once

namespace whirl {

struct EmptyMessage {
  // No fields

  template <typename Archive>
  void serialize(Archive& /*unused*/) {
  }
};

}  // namespace whirl

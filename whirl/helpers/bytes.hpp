#pragma once

#include <whirl/helpers/serialize.hpp>

namespace whirl {

class Bytes {
 public:
  Bytes(std::string bytes) : bytes_(std::move(bytes)) {
  }

  Bytes() = default;

  template <typename T>
  static Bytes Serialize(const T& value) {
    return {::whirl::Serialize<T>(value)};
  }

  template <typename T>
  T As() const {
    return Deserialize<T>(bytes_);
  }

  const std::string& Raw() const {
    return bytes_;
  }

  WHIRL_SERIALIZE(bytes_)

 private:
  std::string bytes_;
};

}  // namespace whirl

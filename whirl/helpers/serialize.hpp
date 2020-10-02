#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <whirl/matrix/allocator.hpp>

#include <sstream>

namespace whirl {

// Helpers for 'cereal' library
// https://github.com/USCiLab/cereal

//////////////////////////////////////////////////////////////////////

static const auto kJsonOutputOptions =
    cereal::JSONOutputArchive::Options().Default();

template <typename T>
std::string Serialize(const T& object) {
  GlobalHeapScope g;

  std::stringstream output;
  {
    cereal::JSONOutputArchive oarchive(output, kJsonOutputOptions);
    oarchive(object);
  }  // archive goes out of scope, ensuring all contents are flushed

  return output.str();
}

template <typename T>
T Deserialize(const std::string& bytes) {
  GlobalHeapScope g;

  T object;

  std::stringstream input(bytes);
  {
    cereal::JSONInputArchive iarchive(input);  // Create an input archive
    iarchive(object);                          // Read the data from the archive
  }

  return std::move(object);
}

//////////////////////////////////////////////////////////////////////

#define SERIALIZE(...)         \
  template <typename Archive>  \
  void serialize(Archive& a) { \
    a(__VA_ARGS__);            \
  };

}  // namespace whirl

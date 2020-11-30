#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

/*
#include <whirl/matrix/memory/allocator.hpp>
#include <whirl/matrix/memory/copy.hpp>
*/

#include <sstream>

namespace whirl {

// Helpers for 'cereal' library
// https://github.com/USCiLab/cereal

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

struct Archives {
  using OutputArchive = cereal::JSONOutputArchive;
  using InputArchive = cereal::JSONInputArchive;
};

#else

struct Archives {
  using OutputArchive = cereal::BinaryOutputArchive;
  using InputArchive = cereal::BinaryInputArchive;
};

#endif

//////////////////////////////////////////////////////////////////////

static const auto kJsonOutputOptions =
    cereal::JSONOutputArchive::Options().Default();

template <typename T>
std::string Serialize(const T& object) {
  // GlobalHeapScope g;

  std::stringstream output;
  {
    Archives::OutputArchive oarchive(output);
    oarchive(object);
  }  // archive goes out of scope, ensuring all contents are flushed

  auto str = output.str();
  return str;

  // return CopyToHeap(str, g.ParentScopeHeap());
}

template <typename T>
T Deserialize(const std::string& bytes) {
  // GlobalHeapScope g;

  T object;

  std::stringstream input(bytes);
  {
    Archives::InputArchive iarchive(input);
    iarchive(object);  // Read the data from the archive
  }

  return object;

  // return CopyToHeap(object, g.ParentScopeHeap());
}

//////////////////////////////////////////////////////////////////////

#define SERIALIZE(...)         \
  template <typename Archive>  \
  void serialize(Archive& a) { \
    a(__VA_ARGS__);            \
  };

#define WHIRL_SERIALIZE(...) SERIALIZE(__VA_ARGS__)

}  // namespace whirl

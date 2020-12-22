#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <ctti/type_id.hpp>

#include <fmt/core.h>

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

namespace detail {

struct SerializedObjectHeader {
  std::string type_name;

  template <typename Archive>
  void serialize(Archive& a) {
    a(CEREAL_NVP(type_name));
  }
};

template <typename T>
std::string GetTypeName() {
  const auto name = ctti::nameof<T>();
  std::stringstream out;
  out << name;
  return out.str();
}

}  // namespace detail

template <typename T>
std::string Serialize(const T& object) {
  // GlobalHeapScope g;

  // Prepare header
  detail::SerializedObjectHeader header;
  header.type_name = detail::GetTypeName<T>();

  std::stringstream output;
  {
    Archives::OutputArchive oarchive(output);
    oarchive(CEREAL_NVP(header));
    oarchive(CEREAL_NVP(object));
  }  // archive goes out of scope, ensuring all contents are flushed

  auto str = output.str();
  return str;

  // return CopyToHeap(str, g.ParentScopeHeap());
}

template <typename T>
T Deserialize(const std::string& bytes) {
  // GlobalHeapScope g;

  const auto type_name = detail::GetTypeName<T>();

  detail::SerializedObjectHeader header;
  T object;

  std::stringstream input(bytes);
  {
    Archives::InputArchive iarchive(input);
    iarchive(header);

    if (header.type_name != type_name) {
      throw std::runtime_error(
          fmt::format("Cannot deserialize value of type '{}', serialized value has type '{}'", type_name, header.type_name));
    }

    iarchive(object);  // Read the data from the archive
  }

  return object;

  // return CopyToHeap(object, g.ParentScopeHeap());
}

//////////////////////////////////////////////////////////////////////

#define WHIRL_SERIALIZE(...)         \
  template <typename Archive>  \
  void serialize(Archive& a) { \
    a(__VA_ARGS__);            \
  };

#define SERIALIZE(...) WHIRL_SERIALIZE(__VA_ARGS__)

}  // namespace whirl

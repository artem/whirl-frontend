#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/string.hpp>
#include <ctti/type_id.hpp>

#include <fmt/core.h>

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
void Serialize(const T& object, std::ostream& output) {
  // Prepare header
  detail::SerializedObjectHeader header;
  header.type_name = detail::GetTypeName<T>();

  {
    Archives::OutputArchive oarchive(output);
    oarchive(CEREAL_NVP(header));
    oarchive(CEREAL_NVP(object));
  }  // archive goes out of scope, ensuring all contents are flushed
}

template <typename T>
std::string Serialize(const T& object) {
  std::stringstream sout;
  Serialize(object, sout);
  return sout.str();
}

template <typename T>
T Deserialize(std::istream& input) {
  const auto type_name = detail::GetTypeName<T>();

  detail::SerializedObjectHeader header;
  T object;

  {
    Archives::InputArchive iarchive(input);
    iarchive(header);

    if (header.type_name != type_name) {
      throw std::runtime_error(
          fmt::format("Cannot deserialize value of type '{}', serialized value "
                      "has type '{}'",
                      type_name, header.type_name));
    }

    iarchive(object);  // Read the data from the archive
  }

  return object;
}

template <typename T>
T Deserialize(const std::string& str) {
  std::stringstream sinput(str);
  return Deserialize<T>(sinput);
}

//////////////////////////////////////////////////////////////////////

}  // namespace whirl

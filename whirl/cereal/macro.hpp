#pragma once

#include <wheels/support/preprocessor.hpp>

#include <cereal/cereal.hpp>

#define WHIRL_SERIALIZE(...)              \
  template <typename Archive>             \
  void serialize(Archive& a) {            \
    a(MAP_LIST(CEREAL_NVP, __VA_ARGS__)); \
  };

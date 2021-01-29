#pragma once

#include <wheels/support/preprocessor.hpp>

#include <cereal/cereal.hpp>

/* Usage:
 * struct MyMessage {
 *   int a;
 *   std::string b;
 *
 *   WHIRL_SERIALIZE(a, b)
 * };
 */

#define WHIRL_SERIALIZE(...)              \
  template <typename Archive>             \
  void serialize(Archive& a) {            \
    a(MAP_LIST(CEREAL_NVP, __VA_ARGS__)); \
  }

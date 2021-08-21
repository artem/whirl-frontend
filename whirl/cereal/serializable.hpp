#pragma once

#include <wheels/support/preprocessor.hpp>

#include <cereal/cereal.hpp>

/* Usage:
 * struct MyMessage {
 *   int32_t a;
 *   std::string b;
 *
 *   WHIRL_SERIALIZABLE(a, b)
 * };
 */

#define WHIRL_SERIALIZABLE(...)           \
  template <typename Archive>             \
  void serialize(Archive& a) {            \
    a(MAP_LIST(CEREAL_NVP, __VA_ARGS__)); \
  }

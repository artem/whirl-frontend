#pragma once

#include <cstdint>
#include <string>

namespace whirl::node::cfg {

using Key = std::string_view;

struct IConfig {
  virtual ~IConfig() = default;

  // Strings

  virtual std::string GetString(Key key) const = 0;

  // Integers

  virtual int64_t GetInt64(Key key) const = 0;

  // Usage: Get<uint16_t>("port")
  template <typename TInteger>
  TInteger GetInt(Key key) const {
    return static_cast<TInteger>(GetInt64(key));
  }

  // Booleans

  virtual bool GetBool(Key key) const = 0;
};

}  // namespace whirl::node::cfg

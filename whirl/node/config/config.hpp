#pragma once

#include <cstdint>
#include <string>

namespace whirl::node::cfg {

struct IConfig {
  virtual ~IConfig() = default;

  virtual std::string GetString(std::string_view key) const = 0;

  virtual int64_t GetInt64(std::string_view key) const = 0;

  // Usage: Get<uint16_t>("port")
  template <typename TInteger>
  TInteger GetInt(std::string_view key) const {
    return static_cast<TInteger>(GetInt64(key));
  }

  virtual bool GetBool(std::string_view key) const = 0;
};

}  // namespace whirl::node::cfg

#pragma once

#include <cstdint>
#include <string>

namespace whirl::node::cfg {

struct IConfig {
  virtual ~IConfig() = default;

  virtual std::string GetString(std::string_view key) const = 0;
  virtual int64_t GetInt64(std::string_view key) const = 0;
  virtual bool GetBool(std::string_view key) const = 0;
};

}  // namespace whirl::node::cfg


#pragma once

#include <whirl/node/config/config.hpp>

#include <whirl/engines/matrix/config/server.hpp>

namespace whirl::matrix::conf {

class NodeConfig : public node::cfg::IConfig {
 public:
  NodeConfig(ServerConfig server_config): server_(server_config) {
  }

  std::string GetString(std::string_view key) const override {
    if (key == "pool") {
      return server_.pool;
    }
    if (key == "db.path") {
      return "/db";
    }
    KeyNotFound(key);
  }

  int64_t GetInt64(std::string_view key) const override {
    if (key == "node.id") {
      return server_.id;
    }
    if (key == "rpc.port") {
      return 42;
    }
    KeyNotFound(key);
  }

  bool GetBool(std::string_view key) const override {
    KeyNotFound(key);
  }

 private:
  [[noreturn]] static void KeyNotFound(std::string_view /*key*/) {
    throw std::runtime_error("Key not found");
  }

 private:
  ServerConfig server_;
};

}  // namespace whirl::matrix::conf
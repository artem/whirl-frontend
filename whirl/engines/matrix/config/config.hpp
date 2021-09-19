#pragma once

#include <whirl/node/config/config.hpp>

#include <whirl/engines/matrix/config/server.hpp>

#include <whirl/engines/matrix/world/global/time_model.hpp>

namespace whirl::matrix::cfg {

class NodeConfig : public node::cfg::IConfig {
 public:
  NodeConfig(ServerConfig server_config) : server_(server_config) {
  }

  std::string GetString(std::string_view key) const override {
    // This server

    if (key == "pool") {
      return server_.pool;
    }

    // Global constant

    if (key == "db.path") {
      return "/db";
    }
    KeyNotFound(key);
  }

  int64_t GetInt64(std::string_view key) const override {
    // This server

    if (key == "node.id") {
      return server_.id;
    }

    // Global constant

    if (key == "rpc.port") {
      return 42;
    }

    // Backoff params
    // Defined by time model

    if (key == "rpc.backoff.init") {
      return TimeModel()->BackoffParams().init;
    }
    if (key == "rpc.backoff.max") {
      return TimeModel()->BackoffParams().max;
    }
    if (key == "rpc.backoff.factor") {
      return TimeModel()->BackoffParams().factor;
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

}  // namespace whirl::matrix::cfg

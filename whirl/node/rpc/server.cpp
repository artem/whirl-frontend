#include <whirl/node/rpc/server.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

#include <commute/rpc/server_impl.hpp>

#include <string>

namespace whirl::node::rpc {

commute::rpc::IServerPtr MakeServer(uint16_t port) {
  const auto port_str = std::to_string(port);

  return std::make_shared<commute::rpc::ServerImpl>(
      port_str, rt::NetTransport(), rt::Executor(), rt::FiberManager(),
      rt::LoggerBackend());
}

}  // namespace whirl::node::rpc

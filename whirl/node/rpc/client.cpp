#include <whirl/node/rpc/client.hpp>

#include <whirl/node/runtime/shortcuts.hpp>

namespace whirl::node::rpc {

commute::rpc::IClientPtr MakeClient() {
  return commute::rpc::MakeClient(rt::NetTransport(), rt::Executor(), rt::LoggerBackend());
}

}  // namespace whirl::node::rpc

#include <whirl/runtime/methods.hpp>

#include <whirl/rpc/server_impl.hpp>
#include <whirl/rpc/client.hpp>

namespace whirl::node::rt {

rpc::IServerPtr MakeRpcServer() {
  return std::make_shared<rpc::ServerImpl>(NetTransport(), Executor(),
                                           FiberManager());
}

rpc::IClientPtr MakeRpcClient() {
  return rpc::MakeClient(NetTransport(), Executor());
}

}  // namespace whirl::node::rt
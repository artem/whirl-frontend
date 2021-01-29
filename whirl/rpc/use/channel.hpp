#pragma once

#include <whirl/rpc/impl/call.hpp>

namespace whirl::rpc {

// T stands for Typed

//////////////////////////////////////////////////////////////////////

// Usage:
// 1) Future<std::string> f = channel.Call("Echo.Echo", data);
// 2) auto f = channel.Call("Echo.Echo", data).As<std::string>();

class TChannel {
 public:
  TChannel(IChannelPtr impl = nullptr) : impl_(impl) {
  }

  template <typename... Arguments>
  detail::CallResult Call(const std::string& method_str,
                          Arguments&&... arguments) {
    return rpc::Call(impl_, method_str, std::forward<Arguments>(arguments)...);
  }

  // Represent peer
  const std::string& Peer() const {
    return impl_->Peer();
  }

  void Close() {
    impl_->Close();
  }

 private:
  IChannelPtr impl_;
};

}  // namespace whirl::rpc

#pragma once

#include <string>
#include <system_error>

//////////////////////////////////////////////////////////////////////

// Error codes

enum class RPCErrorCode {
  Ok = 0,
  ChannelClosed = 1,
  TransportError = 2,
  ExecutionError = 3,
  ServiceNotFound = 4,
  MethodNotFound = 5,
  BadRequest = 6,
  InternalError = 7,
  Cancelled = 8,
};

//////////////////////////////////////////////////////////////////////

namespace whirl::rpc {

bool IsRetriableError(RPCErrorCode error);

}  // namespace whirl::rpc

//////////////////////////////////////////////////////////////////////

// std::error_code support

namespace std {

template <>
struct is_error_code_enum<RPCErrorCode> : true_type {};

}  // namespace std

namespace detail {

class RPCErrorCategory : public std::error_category {
 public:
  const char* name() const noexcept override final {
    return "RPCError";
  }

  std::string message(int c) const override final {
    switch (static_cast<RPCErrorCode>(c)) {
      case RPCErrorCode::Ok: return "RPC ok";
      case RPCErrorCode::ChannelClosed: return "RPC channel closed";
      case RPCErrorCode::TransportError: return "RPC transport error";
      case RPCErrorCode::ExecutionError: return "RPC execution error";
      case RPCErrorCode::ServiceNotFound: return "RPC service not found";
      case RPCErrorCode::MethodNotFound: return "RPC method not found";
      case RPCErrorCode::BadRequest: return "Bad RPC request";
      case RPCErrorCode::InternalError: return "RPC internal error";
      case RPCErrorCode::Cancelled: return "RPC request cancelled";
      default: return "?";
    }
  }

  static const RPCErrorCategory& Get() {
    static const RPCErrorCategory instance;
    return instance;
  }
};

}  // namespace detail

// NOLINTNEXTLINE
inline std::error_code make_error_code(RPCErrorCode e) {
  return {static_cast<int>(e), detail::RPCErrorCategory::Get()};
}

#pragma once

#include <system_error>

//////////////////////////////////////////////////////////////////////

// Error codes

enum class RPCErrorCode {
  Ok = 0,
  TransportError = 1,
  ExecError = 2,
};

//////////////////////////////////////////////////////////////////////

namespace whirl::rpc {

bool IsRetriableError(RPCErrorCode error);

}  // namespace whirl::rpc

//////////////////////////////////////////////////////////////////////

// std::error_code support

namespace std {

template<>
struct is_error_code_enum<RPCErrorCode> : true_type {};

}  // namespace std

namespace detail {

class RPCErrorCategory : public std::error_category
{
 public:
  const char *name() const noexcept override final {
    return "RPCError";
  }

  std::string message(int c) const override final {
    switch (static_cast<RPCErrorCode>(c))
    {
      case RPCErrorCode::Ok:
        return "RPC ok";
      case RPCErrorCode::TransportError:
        return "Transport Error";
      case RPCErrorCode::ExecError:
        return "RPC failed to execute";
      default:
        return "?";
    }
  }
};

}

inline std::error_code make_error_code(RPCErrorCode e) {
  return {static_cast<int>(e), detail::RPCErrorCategory()};
}

#pragma once

#include <whirl/node/rpc/method.hpp>
#include <whirl/node/rpc/id.hpp>
#include <whirl/node/rpc/bytes_value.hpp>
#include <whirl/node/rpc/trace.hpp>
#include <whirl/node/rpc/errors.hpp>

#include <whirl/cereal/serializable.hpp>

#include <cereal/types/string.hpp>

#include <string>

namespace whirl::rpc {

// Network protocol

namespace proto {

//////////////////////////////////////////////////////////////////////

struct Request {
  RequestId id;
  rpc::TraceId trace_id;
  std::string to;  // For debugging
  Method method;
  BytesValue input;  // std::tuple

  WHIRL_SERIALIZABLE(id, trace_id, to, method, input)
};

//////////////////////////////////////////////////////////////////////

struct Response {
  RequestId request_id;
  Method method;      // For debugging, echoes that of the request
  BytesValue result;  // wheels::Unit for handlers with void return type
  RPCErrorCode error;

  WHIRL_SERIALIZABLE(request_id, method, result, error)

  bool IsOk() const {
    return error == RPCErrorCode::Ok;
  }
};

}  // namespace proto

}  // namespace whirl::rpc

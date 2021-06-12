#pragma once

#include <whirl/rpc/method.hpp>
#include <whirl/rpc/id.hpp>
#include <whirl/rpc/bytes_value.hpp>
#include <whirl/rpc/trace.hpp>
#include <whirl/rpc/errors.hpp>

#include <whirl/cereal/serializable.hpp>

#include <cereal/types/string.hpp>

#include <string>

namespace whirl::rpc {

// Network protocol

//////////////////////////////////////////////////////////////////////

// Request

struct RequestMessage {
  RequestId id;
  TraceId trace_id;
  std::string to;  // For debugging
  Method method;
  BytesValue input;  // std::tuple

  WHIRL_SERIALIZE(id, trace_id, to, method, input)
};

//////////////////////////////////////////////////////////////////////

// Response

struct ResponseMessage {
  RequestId request_id;
  Method method;      // For debugging, echoes that of the request
  BytesValue result;  // wheels::Unit for handlers with void return type
  RPCErrorCode error;

  WHIRL_SERIALIZE(request_id, method, result, error)

  bool IsOk() const {
    return error == RPCErrorCode::Ok;
  }
};

}  // namespace whirl::rpc

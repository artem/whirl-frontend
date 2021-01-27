#pragma once

#include <whirl/rpc/impl/method.hpp>
#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/bytes_value.hpp>
#include <whirl/rpc/impl/trace.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/cereal/serialize.hpp>

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
  BytesValue input;

  WHIRL_SERIALIZE(id, trace_id, to, method, input)
};

//////////////////////////////////////////////////////////////////////

// Response

struct ResponseMessage {
  RequestId request_id;
  Method method;  // For debugging, echoes that of the request
  BytesValue result;
  RPCErrorCode error;

  WHIRL_SERIALIZE(request_id, method, result, error)

  bool IsOk() const {
    return error == RPCErrorCode::Ok;
  }
};

}  // namespace whirl::rpc

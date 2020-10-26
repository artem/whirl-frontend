#pragma once

#include <whirl/rpc/impl/method.hpp>
#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/bytes_value.hpp>
#include <whirl/rpc/impl/trace.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/helpers/serialize.hpp>

#include <cereal/types/string.hpp>

#include <string>

namespace whirl::rpc {

// Network protocol

//////////////////////////////////////////////////////////////////////

// Request

struct RequestMessage {
  RPCId id;
  TraceId trace_id;
  std::string to;  // For debugging
  Method method;
  BytesValue input;

  SERIALIZE(CEREAL_NVP(id), CEREAL_NVP(trace_id), CEREAL_NVP(to),
            CEREAL_NVP(method), CEREAL_NVP(input))
};

//////////////////////////////////////////////////////////////////////

// Response

struct ResponseMessage {
  RPCId request_id;
  Method method;  // For debugging
  BytesValue result;
  RPCErrorCode error;

  SERIALIZE(CEREAL_NVP(request_id), CEREAL_NVP(method), CEREAL_NVP(result),
            CEREAL_NVP(error))

  bool IsOk() const {
    return error == RPCErrorCode::Ok;
  }
};

}  // namespace whirl::rpc

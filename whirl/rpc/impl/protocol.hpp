#pragma once

#include <whirl/rpc/impl/id.hpp>
#include <whirl/rpc/impl/raw_value.hpp>
#include <whirl/rpc/impl/trace.hpp>
#include <whirl/rpc/impl/errors.hpp>

#include <whirl/helpers/serialize.hpp>

#include <cereal/types/string.hpp>

#include <string>

namespace whirl::rpc {

// Network protocol

//////////////////////////////////////////////////////////////////////

// Request

struct RPCRequestMessage {
  RPCId id;
  TraceId trace_id;
  std::string target;  // For debugging
  std::string method;
  BytesValue input;

  SERIALIZE(CEREAL_NVP(id), CEREAL_NVP(trace_id), CEREAL_NVP(target),
            CEREAL_NVP(method), CEREAL_NVP(input))
};

//////////////////////////////////////////////////////////////////////

// Response

struct RPCResponseMessage {
  RPCId request_id;
  std::string method;  // For debugging
  BytesValue result;
  RPCErrorCode error;

  SERIALIZE(CEREAL_NVP(request_id), CEREAL_NVP(method), CEREAL_NVP(result),
            CEREAL_NVP(error))

  bool IsOk() const {
    return error == RPCErrorCode::Ok;
  }
};

}  // namespace whirl::rpc

#pragma once

#include <whirl/engines/matrix/world/global/global.hpp>

#include <whirl/cereal/serialize.hpp>
#include <whirl/cereal/tuple.hpp>

#define WHIRL_RECORDED_CALL(obj, method, ...)                    \
  auto cookie = GetHistoryRecorder().CallStarted(                \
      TO_STRING(method), SerializeValues(__VA_ARGS__));          \
  auto result = obj.method(__VA_ARGS__);                         \
  GetHistoryRecorder().CallCompleted(cookie, Serialize(result)); \
  return result;

#define WHIRL_RECORDED_VOID_CALL(obj, method, ...)      \
  auto cookie = GetHistoryRecorder().CallStarted(       \
      TO_STRING(method), SerializeValues(__VA_ARGS__)); \
  obj.method(__VA_ARGS__);                              \
  GetHistoryRecorder().CallCompleted(cookie, "");       \
  return;

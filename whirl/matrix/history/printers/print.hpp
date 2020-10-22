#pragma once

#include <whirl/matrix/history/history.hpp>

#include <string>
#include <set>

namespace whirl::histories {

//////////////////////////////////////////////////////////////////////

static const size_t kUnitLength = 3;

inline std::string PrintLabels(const CallLabels& labels) {
  if (labels.empty()) {
    return "";
  }

  std::stringstream out;
  out << "{";
  for (size_t i = 0; i < labels.size(); ++i) {
    out << labels[i];
    if (i + 1 < labels.size()) {
      out << ", ";
    }
  }
  out << "}";
  return out.str();
}

inline std::string MakeSpace(TimePoint start_ts) {
  return std::string(start_ts * kUnitLength, ' ');
};

inline std::string MakeCallSegment(TimePoint start, TimePoint end,
                                   bool completed) {
  std::string call((end - start) * kUnitLength, '-');
  call[0] = '[';
  if (completed) {
    call[call.length() - 1] = ']';
  }
  return call;
}

//////////////////////////////////////////////////////////////////////

template <typename CallPrinter>
void Print(const History& history) {
  // Collect time points

  std::set<TimePoint> tps;
  for (const auto& call : history) {
    tps.insert(call.start_time);
    if (call.IsCompleted()) {
      tps.insert(*call.end_time);
    }
  }

  // Compact time points

  std::map<TimePoint, TimePoint> compact_tps;
  size_t ctp = 0;
  for (auto tp : tps) {
    ++ctp;
    compact_tps[tp] = ctp;
  }
  size_t max_tp = ctp + 1;

  for (size_t i = 0; i < history.size(); ++i) {
    const auto& call = history[i];

    TimePoint start_ts = compact_tps[call.start_time];

    TimePoint end_ts;
    if (call.IsCompleted()) {
      end_ts = compact_tps[*call.end_time];
    } else {
      end_ts = max_tp;
    }

    std::cout << MakeSpace(start_ts) << (i + 1) << ". "
              << CallPrinter::Print(call) << "\t" << PrintLabels(call.labels) << std::endl;

    std::cout << MakeSpace(start_ts)
              << MakeCallSegment(start_ts, end_ts, call.IsCompleted())
              << std::endl;
  }
}

}  // namespace whirl::histories

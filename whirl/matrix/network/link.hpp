#pragma once

#include <whirl/time.hpp>
#include <whirl/matrix/network/packet.hpp>

#include <whirl/helpers/priority_queue.hpp>

#include <whirl/matrix/log/logger.hpp>

namespace whirl {

// One-way link
class Link {
 private:
  using ServerName = std::string;

  struct PacketEvent {
    NetPacket packet;
    TimePoint time;

    bool operator<(const PacketEvent& that) const {
      return time < that.time;
    }
  };

  using PacketQueue = PriorityQueue<PacketEvent>;

 public:
  Link(ServerName start, ServerName end) : start_(start), end_(end) {
  }

  const ServerName& Start() const {
    return start_;
  }

  const ServerName& End() const {
    return end_;
  }

  void SetOpposite(Link* link) {
    opposite_ = link;
  }

  Link* GetOpposite() const {
    return opposite_;
  }

  bool IsLoopBack() const {
    return start_ == end_;
  }

  void Add(NetPacket packet);

  bool IsPaused() const {
    return paused_;
  }

  bool HasPackets() const {
    return !packets_.IsEmpty();
  }

  std::optional<TimePoint> NextPacketTime() const {
    if (!paused_ && HasPackets()) {
      return packets_.Smallest().time;
    }
    return std::nullopt;
  }

  NetPacket ExtractNextPacket();

  void Shutdown() {
    packets_.Clear();
  }

  // Faults

  void Pause();
  void Resume();

 private:
  TimePoint ChoosePacketDeliveryTime() const;

 private:
  ServerName start_;
  ServerName end_;

  PacketQueue packets_;
  bool paused_{false};

  Link* opposite_{nullptr};

  Logger logger_{"Network"};
};

}  // namespace whirl

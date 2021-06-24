#pragma once

#include <whirl/time.hpp>
#include <whirl/engines/matrix/network/packet.hpp>
#include <whirl/engines/matrix/network/server.hpp>

#include <whirl/helpers/priority_queue.hpp>

#include <whirl/logger/log.hpp>

namespace whirl::matrix::net {

class Network;

// One-way link
class Link {
 private:
  struct PacketEvent {
    Packet packet;
    TimePoint time;

    bool operator<(const PacketEvent& that) const {
      return time < that.time;
    }
  };

  using PacketQueue = PriorityQueue<PacketEvent>;

 public:
  Link(Network* net, IServer* start, IServer* end)
      : net_(net), start_(start), end_(end) {
  }

  IServer* Start() const {
    return start_;
  }

  IServer* End() const {
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

  void Add(Packet packet);

  bool IsPaused() const {
    return paused_;
  }

  bool HasPackets() const {
    return !packets_.IsEmpty();
  }

  TimePoint NextPacketTime() const {
    return packets_.Smallest().time;
  }

  Packet ExtractNextPacket();

  void Shutdown() {
    packets_.Clear();
  }

  // Faults

  void Pause();
  void Resume();

 private:
  TimePoint ChooseDeliveryTime(const Packet& packet) const;
  void Add(Packet&& packet, TimePoint delivery_time);

 private:
  Network* net_;
  IServer* start_;
  IServer* end_;

  PacketQueue packets_;
  bool paused_{false};

  Link* opposite_{nullptr};

  Logger logger_{"Network"};
};

}  // namespace whirl::matrix::net

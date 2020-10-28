#include <whirl/matrix/network/link_layer.hpp>

#include <whirl/matrix/world/global.hpp>

#include <wheels/support/compiler.hpp>

namespace whirl {

void LinkLayer::AddServer(const ServerName& server) {
  servers_.push_back(server);
}

void LinkLayer::BuildLinks() {
  for (size_t i = 0; i < servers_.size(); ++i) {
    for (size_t j = 0; j < servers_.size(); ++j) {
      links_.emplace_back(servers_[i], servers_[j]);

      if (i >= j) {
        size_t ij = GetLinkIndex(i, j);
        size_t ji = GetLinkIndex(j, i);

        links_[ij].SetOpposite(&links_[ji]);
        links_[ji].SetOpposite(&links_[ij]);
      }
    }
  }
}

Link* LinkLayer::GetLink(const ServerName& start, const ServerName& end) {
  size_t i = ServerToIndex(start);
  size_t j = ServerToIndex(end);

  return &links_.at(GetLinkIndex(i, j));
}

bool LinkLayer::IsRunnable() const {
  for (const auto& link : links_) {
    if (!link.IsPaused() && link.HasPackets()) {
      return true;
    }
  }
  return false;
}

Link* LinkLayer::FindLinkWithNextPacket() {
  Link* next = nullptr;

  for (auto& link : links_) {
    if (link.IsPaused() || !link.HasPackets()) {
      continue;
    }
    if (!next) {
      next = &link;
      continue;
    }

    // i - active, next - not empty
    if (*(link.NextPacketTime()) < *(next->NextPacketTime())) {
      next = &link;
    }
  }

  return next;
}

void LinkLayer::Shutdown() {
  for (auto& link : links_) {
    link.Shutdown();
  }
}

size_t LinkLayer::ServerToIndex(const ServerName& server) const {
  for (size_t i = 0; i < servers_.size(); ++i) {
    if (server == servers_[i]) {
      return i;
    }
  }
  WHEELS_UNREACHABLE();
}

size_t LinkLayer::GetLinkIndex(size_t i, size_t j) const {
  return i * servers_.size() + j;
}

// Partitions

static bool Cross(const Link& link, const Partition& lhs) {
  return lhs.count(link.Start()) != lhs.count(link.End());
}

void LinkLayer::Split(const Partition& lhs) {
  for (auto& link : links_) {
    if (Cross(link, lhs)) {
      link.Pause();
    }
  }
}

void LinkLayer::Heal() {
  for (auto& link : links_) {
    link.Resume();
  }
}

}  // namespace whirl

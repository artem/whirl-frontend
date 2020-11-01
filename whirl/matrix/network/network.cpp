#include <whirl/matrix/network/network.hpp>

#include <whirl/matrix/common/allocator.hpp>
#include <whirl/matrix/world/global.hpp>

#include <wheels/support/compiler.hpp>

namespace whirl::net {

void Network::AddServer(INetServer* server) {
  servers_.push_back(server);
}

void Network::BuildLinks() {
  // Create one-way link between each pair of servers
  for (size_t i = 0; i < servers_.size(); ++i) {
    for (size_t j = 0; j < servers_.size(); ++j) {
      links_.emplace_back(servers_[i], servers_[j]);
    }
  }

  // Link opposite physical links
  for (size_t i = 0; i < servers_.size(); ++i) {
    for (size_t j = i; j < servers_.size(); ++j) {
      size_t ij = GetLinkIndex(i, j);
      size_t ji = GetLinkIndex(j, i);

      links_[ij].SetOpposite(&links_[ji]);
      links_[ji].SetOpposite(&links_[ij]);
    }
  }
}

Link* Network::GetLink(const HostName& start, const HostName& end) {
  size_t i = ServerToIndex(start);
  size_t j = ServerToIndex(end);

  return &links_.at(GetLinkIndex(i, j));
}

// IActor

void Network::Start() {
  BuildLinks();
}

bool Network::IsRunnable() const {
  for (const auto& link : links_) {
    if (!link.IsPaused() && link.HasPackets()) {
      return true;
    }
  }
  return false;
}

Link* Network::FindLinkWithNextPacket() {
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

TimePoint Network::NextStepTime() {
  Link* link = FindLinkWithNextPacket();
  return *link->NextPacketTime();
}

void Network::Step() {
  Link* link = FindLinkWithNextPacket();
  Packet packet = link->ExtractNextPacket();
  link->End()->HandlePacket(packet, link->GetOpposite());
}

void Network::Shutdown() {
  for (auto& link : links_) {
    link.Shutdown();
  }
}

size_t Network::ServerToIndex(const HostName& hostname) const {
  for (size_t i = 0; i < servers_.size(); ++i) {
    if (hostname == servers_[i]->HostName()) {
      return i;
    }
  }
  WHEELS_UNREACHABLE();
}

size_t Network::GetLinkIndex(size_t i, size_t j) const {
  return i * servers_.size() + j;
}

// Partitions

void Network::Split() {
  GlobalHeapScope g;

  // Generate random partition

  // Generate partition
  auto servers = servers_;

  Partition lhs;
  // [1, servers.size())
  size_t lhs_size = GlobalRandomNumber(1, servers.size());

  for (size_t i = 0; i < lhs_size; ++i) {
    size_t k = GlobalRandomNumber(i, lhs_size);
    std::swap(servers[i], servers[k]);
    lhs.insert(servers[i]->HostName());
  }

  // Print
  WHIRL_FMT_LOG("Network partitioned: {} / {}", lhs.size(),
                servers.size() - lhs.size());

  // Split
  Split(lhs);
}

static bool Cross(const Link& link, const Partition& lhs) {
  return lhs.count(link.StartHostName()) != lhs.count(link.EndHostName());
}

void Network::Split(const Partition& lhs) {
  for (auto& link : links_) {
    if (Cross(link, lhs)) {
      link.Pause();
    }
  }
}

void Network::Heal() {
  GlobalHeapScope g;

  for (auto& link : links_) {
    link.Resume();
  }
}

}  // namespace whirl::net

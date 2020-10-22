#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

// TODO

size_t ServerCount();

// Server

std::string GetServerName(size_t index);

void PauseServer(size_t index);
void ResumeServer(size_t index);

void RebootServer(size_t index);

void AdjustServerClock(size_t index);

// Network

}  // namespace whirl

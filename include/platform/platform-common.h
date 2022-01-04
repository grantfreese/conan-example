// platform.h - common header for platform-specific code

#pragma once

// local includes
#include "platform.h"   // loads appropriate header for current platform

// lib includes

// os headers

// standard headers
#include <cstdint>

namespace freese
{

uint64_t getTimeHighRes();
void initPlatform();
void loadSettings();

} // namespace freese

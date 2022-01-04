// platform.h - bare-metal platform-specific code

#pragma once

// local includes
#include "platform/platform-common.h"

// lib includes

// os headers

// standard headers
#include <cstdint>

namespace freese
{

//TODO: add architecture check on this
inline uint16_t be16toh(uint16_t x)
{
    return((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)));
}

//TODO: add architecture check on this
inline uint16_t le16toh(uint16_t x)
{
    return(x);
}

//TODO: add architecture check on this
inline uint16_t htole16(uint16_t x)
{
    return(x);
}


//stub function (posix function that's only used in linux target)
typedef void (*__sighandler_t) (int);
__sighandler_t signal (int __sig, __sighandler_t __handler){};


} // namespace freese

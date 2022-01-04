// baremetal.cpp - baremetal platform-specific code

// local includes

// lib includes


// os headers
#include <chrono>

// standard headers
#include <csignal> // pthread_kill()

using namespace std::chrono;

namespace freese
{

void initPlatform()
{
}

// provide 64-bit timestamp (in ticks of(in ticks of 100ns)
uint64_t getTimeHighRes()
{
    // time_point<high_resolution_clock> timeEpoch;
    // auto timeElapsed = high_resolution_clock::now() - timeEpoch;
    // std::uint64_t timestamp = duration_cast<nanoseconds>(timeElapsed).count() / 100u;
    // return (timestamp);

    // TODO: temporary stub
    return(0);
}

void loadSettings()
{
    
}

} // namespace freese

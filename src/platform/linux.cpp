// linux.cpp - linux platform-specific code

// local includes
#include "platform/platform-common.h"

// lib includes
#include "nlohmann/json.hpp"

// os headers
#include <chrono>
#include <filesystem>
#include <unistd.h> // write(), read(), close(), usleep()
#include "FreeRTOS.h"
#include "task.h"

// standard headers
#include <cassert>
#include <csignal> // pthread_kill()
#include <fstream>
#include <iostream>


using namespace std::chrono;
using json = nlohmann::json;

namespace freese
{



void initPlatform()
{
    // change behavior of 'SIGINT' signal so that it interrupts syscalls and makes them return -1
    siginterrupt(SIGINT, true);
}

// provide 64-bit timestamp (in ticks of(in ticks of 100ns)
uint64_t getTimeHighRes()
{
    time_point<high_resolution_clock> timeEpoch;
    auto timeElapsed = high_resolution_clock::now() - timeEpoch;
    std::uint64_t timestamp = duration_cast<nanoseconds>(timeElapsed).count();
    return (timestamp);
}

void loadSettings()
{
    // verify existence of settings.json
    std::filesystem::path settingsFile{"settings.json"};
    assert(std::filesystem::exists(settingsFile) == true);

    // parse settings.json
    std::ifstream ifs("settings.json");
    json jf = json::parse(ifs);
    std::cout << "settings.json:" << std::endl;
    std::cout << jf.dump(4) << std::endl;

    for (auto& [key,value] : jf.items())
    {
        std::cout << key << "=" << value << std::endl;
    }    
}

} // namespace freese

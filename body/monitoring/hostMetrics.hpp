#pragma once

#include <cstdint>
#include <string>

inline constexpr int SEC_IN_DAY    = 86400;
inline constexpr int SEC_IN_HOUR   = 3600;
inline constexpr int SEC_IN_MINUTE = 60;

struct CpuTimes {
    unsigned long long user = 0, nice = 0, system = 0, idle = 0,
                       iowait = 0, irq = 0, softirq = 0, steal = 0;
};

struct RamParts {
    uint64_t totalMemory = 0, available = 0, used = 0;
    double usedPercent = 0.0;
};

bool readCpuTimes(CpuTimes& times);
bool readRamParts(RamParts& parts);
std::string getUptime();
double getTemp();

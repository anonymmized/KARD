#pragma once

#include <cstdint>
#include <string>

inline constexpr int SEC_IN_DAY    = 86400;
inline constexpr int SEC_IN_HOUR   = 3600;
inline constexpr int SEC_IN_MINUTE = 60;
inline constexpr int TIMEOUT_IN_MS = 1000;

struct CpuTimes {
    unsigned long long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0;
};

struct RamParts {
    uint64_t total_memory = 0, available = 0, used = 0;
    double used_percent = 0.0;
};

bool readCpuTimes(CpuTimes &t);
double computeUsage(const CpuTimes& prev, const CpuTimes& curr);
double getCpuUsage(int delay_ms = 200);
bool readRamParts(RamParts& parts);
double getRamUsage();
std::string getDiskSpace();
std::string getUptime();
std::string getAll();
double getTemp();
std::string summarizeHealth(const int hours);

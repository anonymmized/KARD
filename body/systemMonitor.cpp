#include "systemMonitor.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/sysctl.h>
#else
#include <fstream>
#include <sstream>
#endif

#ifdef __APPLE__
bool readCpuTimes(CpuTimes &t) {
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) != KERN_SUCCESS) return false;
    t.user = cpuinfo.cpu_ticks[CPU_STATE_USER];
    t.nice = cpuinfo.cpu_ticks[CPU_STATE_NICE];
    t.system = cpuinfo.cpu_ticks[CPU_STATE_SYSTEM];
    t.idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
    return true;

}
#else
bool readCpuTimes(CpuTimes &t) {
    std::ifstream fl("/proc/stat");
    if (!fl.is_open()) return false;
    std::string line;
    std::getline(fl, line);
    std::istringstream ss(line);
    std::string cpuLabel;
    ss >> cpuLabel;
    ss >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >> t.softirq >> t.steal;
    return true;
}
#endif

double computeUsage(const CpuTimes& prev, const CpuTimes& curr) {
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;
    unsigned long long prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    unsigned long long currNonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;
    unsigned long long prevTotal = prevIdle + prevNonIdle;
    unsigned long long currTotal = currIdle + currNonIdle;

    unsigned long long totald = currTotal - prevTotal;
    unsigned long long idled = currIdle - prevIdle;

    if (totald == 0) return 0.0;
    double cpu_percentage = (double)(totald - idled) * 100.0 / (double)totald;
    if (cpu_percentage < 0.0) cpu_percentage = 0.0;
    if (cpu_percentage > 100.0) cpu_percentage = 100.0;
    return cpu_percentage;
}

double getCpuUsage(int delay_ms) {
    CpuTimes prev{};
    CpuTimes curr{};
    if (!readCpuTimes(prev)) {
        if (!readCpuTimes(curr)) return -1.0;
        prev = curr;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        if (!readCpuTimes(curr)) return -1.0;
        return computeUsage(prev, curr);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    if (!readCpuTimes(curr)) return -1.0;
    double usage = computeUsage(prev, curr);
    prev = curr;

    return usage;
}

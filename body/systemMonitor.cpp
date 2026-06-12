#include "systemMonitor.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <stdexcept>

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/time.h>
#else
#include <unordered_map>
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
bool readRamParts(RamParts& parts) {
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, reinterpret_cast<host_info_t>(&vmstat), &count) != KERN_SUCCESS) return false;

    int64_t page_size;
    host_page_size(mach_host_self(), reinterpret_cast<vm_size_t*>(&page_size));

    uint64_t free_memory = (uint64_t)vmstat.free_count * page_size;
    uint64_t inactive_memory = (uint64_t)vmstat.inactive_count * page_size;
    uint64_t speculative = (uint64_t)vmstat.speculative_count * page_size;
    uint64_t available = free_memory + inactive_memory + speculative;

    uint64_t total_memory = 0;
    size_t len = sizeof(total_memory);
    if (sysctlbyname("hw.memsize", &total_memory, &len, NULL, 0) != 0) return false;

    uint64_t used = total_memory - available;
    double used_percent = (double)used * 100.0 / (double)total_memory;
    parts.total_memory = total_memory;
    parts.available = available;
    parts.used = used;
    parts.used_percent = used_percent;
    return true;
}
std::string getUptime() {
    struct timeval boottime;
    size_t sz = sizeof(boottime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    if (sysctl(mib, 2, &boottime, &sz, nullptr, 0) != 0) return "There is an error in getting the correct time\n";
    time_t boot_sec = boottime.tv_sec;
    time_t now = time(nullptr);
    if (now == (time_t)-1) return "Can't get a current time\n";
    double time_in_sec = difftime(now, boot_sec);
    double days = (int)time_in_sec / 86400;
    double hours = (int)time_in_sec % 86400 / 3600;
    double minutes = (int)time_in_sec % 86400 % 3600 / 60;
    return std::to_string(days) + "d. " + std::to_string(hours) + "h. " + std::to_string(minutes) + "m.";
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
bool readRamParts(RamParts& parts) {
    std::ifstream fl("/proc/meminfo");
    if (!fl.is_open()) return false;

    std::unordered_map<std::string, long long> m;
    std::string key;
    long long value;
    std::string unit;
    while (fl >> key >> value >> unit) {
        if (!key.empty() && key.back()==':') key.pop_back();
        m[key] = value;
    }

    uint64_t total_kb = m["MemTotal"];
    uint64_t available_kb = 0;
    if (m.count("MemAvailable")) available_kb = m["MemAvailable"];
    else available_kb = m["MemFree"] + m["Buffers"] + m["Cached"];

    uint64_t used_kb = (total_kb > available_kb) ? (total_kb - available_kb) : 0;

    const uint64_t KB_TO_BYTES = 1024ULL;
    uint64_t total = total_kb * KB_TO_BYTES;
    uint64_t available = available_kb * KB_TO_BYTES;
    uint64_t used = used_kb * KB_TO_BYTES;
    double used_percent_tms = (total == 0) ? 0.0 : (double)used * 100.0 / (double)total;
    parts.total_memory = total;
    parts.available = available;
    parts.used = used;
    parts.used_percent = used_percent_tms;
    return true;
}
std::string getUptime() {
    std::ifstream fl("/proc/uptime");
    if (!fl.is_open()) return "Failed to open /proc/uptime\n";
    double time_in_sec;
    fl >> time_in_sec;
    double days = time_in_sec / 86400.0;
    double hours = time_in_sec % 86400.0 / 3600.0;
    double minutes = time_in_sec % 86400.0 % 3600.0 / 60.0;
    return std::to_string(days) + "d. " + std::to_string(hours) + "h. " + std::to_string(minutes) + "m.";
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

double getRamUsage() {
    RamParts parts;
    if (!readRamParts(parts)) return -1.0;
    return parts.used_percent;
}

std::string getDiskSpace() {
    auto info = std::filesystem::space("/");
    using ull = unsigned long long;
    ull total = info.capacity;
    ull free = info.available;
    ull used = total - free;
    ull factor = 1024ULL*1024ULL*1024ULL;
    ull total_gib = total / factor;
    ull used_gib = used / factor;
    return std::to_string(used_gib) + "/" + std::to_string(total_gib);
}

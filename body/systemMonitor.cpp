#include "systemMonitor.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <nlohmann/json.hpp>
#include <fstream>
#include <set>
#include <netdb.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/socket.h>

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/time.h>
#else
#include <unordered_map>
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

double getTemp() { return -1.0; }

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
    struct timeval bootTime;
    size_t bootTimeSize = sizeof(bootTime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    if (sysctl(mib, 2, &bootTime, &bootTimeSize, nullptr, 0) != 0) return "There is an error in getting the correct time\n";
    time_t bootSec = bootTime.tv_sec;
    time_t currentTime = time(nullptr);
    if (currentTime == (time_t)-1) return "Can't get a current time\n";
    long timeInSec = (long)difftime(currentTime, bootSec);
    long days = timeInSec / SEC_IN_DAY;
    long hours = (timeInSec % SEC_IN_DAY) / SEC_IN_HOUR;
    long minutes = (timeInSec % SEC_IN_HOUR) / SEC_IN_MINUTE;
    return std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m";
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
    std::ifstream fileToRead("/proc/uptime");
    if (!fileToRead.is_open()) return "Failed to open /proc/uptime\n";
    double timeInSec;
    fileToRead >> timeInSec;
    long totalInSec = (long)timeInSec;
    long days = totalInSec / SEC_IN_DAY;
    long hours = (totalInSec % SEC_IN_DAY) / SEC_IN_HOUR;
    long minutes = (totalInSec % SEC_IN_HOUR) / SEC_IN_MINUTE;
    return std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m";
}

double getTemp() {
    std::ifstream fl("/sys/class/thermal/thermal_zone0/temp");
    if (!fl.is_open()) return -1.0;
    long milli;
    if (!(fl >> milli)) return -1.0;
    return milli / 1000.0;
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
    auto infoAboutSystem = std::filesystem::space("/");
    using ull = unsigned long long;
    ull totalSpace = infoAboutSystem.capacity;
    ull freeSpace = infoAboutSystem.available;
    ull usedSpace = totalSpace - freeSpace;
    ull factor = 1024ULL*1024ULL*1024ULL;
    ull totalSpaceGb = totalSpace / factor;
    ull usedSpaceGb = usedSpace / factor;
    double spaceInPercent = (totalSpace == 0) ? 0.0 : (double)usedSpace * 100.0 / (double)totalSpace;
    return "used " + std::to_string(usedSpaceGb) + " Gb / total " + std::to_string(totalSpaceGb) + " Gb (" + std::to_string((int)(spaceInPercent + 0.5)) + "%)";
}

std::string getAll() {
    double cpu = getCpuUsage();
    double ram = getRamUsage();
    std::string disk = getDiskSpace();
    std::string uptime = getUptime();
    return "cpu_usage_percent: " + std::to_string(cpu)
         + ", ram_used_percent: " + std::to_string(ram)
         + ", disk: " + disk
         + ", uptime: " + uptime;
}

std::string summarizeHealth(const int seconds) {
    std::ifstream fl(LOG_PATH);
    if (!fl.is_open()) {
        return "unable to open log file for reading\n";
    }
    std::string line;
    auto now = std::chrono::system_clock::now();
    auto now_ts = std::chrono::system_clock::to_time_t(now);
    std::unordered_map<std::string, std::vector<std::pair<long, double>>> series;
    while (std::getline(fl, line)) {
        if (line.empty()) continue;
        auto j = nlohmann::json::parse(line, nullptr, false);
        if (j.is_discarded()) continue;

        long ts = j["ts"].get<long>();
        if (now_ts - ts > seconds) continue;
        static const std::set<std::string> numeric = {"get_cpu", "get_ram", "get_temp"};
        std::string tool = j["tool"].get<std::string>();
        if (!numeric.count(tool)) continue;
        try {
            series[tool].push_back({ts, std::stod(j["value"].get<std::string>())});
        } catch (const std::exception&) {
            continue;
        }
    }
    std::string summary;
    for (auto& [key, value] : series) {
        std::sort(value.begin(), value.end(), [](const auto& a, const auto& b){ return a.first < b.first; });
        double sum = 0.0, peak = value.front().second;
        for (const auto& [ts, val] : value) {
            sum += val;
            peak = std::max(peak, val);
        }

        double avg = sum / value.size();
        double growth = value.back().second - value.front().second;

        summary += key + ": avg " + std::to_string(avg) + ", peak " + std::to_string(peak) + ", change " + std::to_string(growth) + " (" + std::to_string(value.size()) + " samples)\n";
    }
    return summary.empty() ? "no data for the last " + std::to_string(seconds / 3600) + "h." : summary;

}

double getTcpProbe(const char* destIp, uint16_t sourcePort) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0) {
        return -1.0;
    }

    fcntl(fileDescriptor, F_SETFL, fcntl(fileDescriptor, F_GETFL, 0) | O_NONBLOCK);

    sockaddr_in workingAddress{};
    workingAddress.sin_family = AF_INET;
    workingAddress.sin_port = htons(sourcePort);
    inet_pton(AF_INET, destIp, &workingAddress.sin_addr);

    double roundTripTime = 0;
    auto tripStart = std::chrono::steady_clock::now();
    int connectAnswer = connect(fileDescriptor, (sockaddr*)&workingAddress, sizeof(workingAddress));
    if (connectAnswer < 0 && errno == EINPROGRESS) {
        fd_set w;
        FD_ZERO(&w);
        FD_SET(fileDescriptor, &w);
        timeval timoutSides{ TIMEOUT_IN_MS / 1000, (TIMEOUT_IN_MS % 1000) * 1000 };

        int selectAnswer = select(fileDescriptor + 1, nullptr, &w, nullptr, &timoutSides) {
            int error = 0;
            socklen_t socketLength = sizeof(error);
            getsockopt(fileDescriptor, SOL_SOCKET, SO_ERROR, &error, &socketLength);
            if (error == 0) {
                roundTripTime = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - tripStart).count();
            }
        }
    }
    close(fileDescriptor);
    return roundTripTime;
}

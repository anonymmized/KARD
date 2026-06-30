#include "hostMetrics.hpp"

#include <mach/mach.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <ctime>
#include <string>

bool readCpuTimes(CpuTimes &times) {
    host_cpu_load_info_data_t cpuInfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO_COUNT, (host_info_t)&cpuInfo, &count) != KERN_SUCCESS) {
        return false;
    }
    times.user = cpuInfo.cpu_ticks[CPU_STATE_USER];
    times.nice = cpuInfo.cpu_ticks[CPU_STATE_NICE];
    times.system = cpuInfo.cpu_ticks[CPU_STATE_SYSTEM];
    times.idle = cpuInfo.cpu_ticks[CPU_STATE_IDLE];
    return true;
}

double getTemp() {
    return -1.0;
}

bool readRamParts(RamParts& parts) {
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if (host_statistics(mach_host_self(), HOST_VM_INFO64, reinterpret_cast<host_info_t>(&vmstat), &count) != KERN_SUCCESS) {
        return false;
    }

    int64_t pageSize;
    host_page_size(mach_host_self(), reinterpret_cast<vm_size_t*>(&pageSize));

    uint64_t freeMem = (uint64_t)vmstat.free_count * pageSize;
    uint64_t inactiveMem = (uint64_t)vmstat.inactive_count * pageSize;
    uint64_t speculative = (uint64_t)vmstat.speculative_count * pageSize;
    uint64_t available = freeMem + inactiveMem + speculative;

    uint64_t totalMem = 0;
    size_t len = sizeof(totalMem);
    if (sysctlbyname("hw.memsize", &totalMem, &len, NULL, 0) != 0) {
        return false;
    }

    uint64_t usedMem = totalMem - available;
    double usedPercent = (double)usedMem * 100.0 / (double)totalMem;
    parts.totalMemory = totalMem;
    parts.available = available;
    parts.used = usedMem;
    parts.usedPercent = usedPercent;
    return true;
}

std::string getUptime() {
    struct timeval bootTime;
    size_t bootTimeSize = sizeof(bootTime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    if (sysctl(mib, 2, &bootTime, &bootTimeSize, nullptr, 0) != 0) {
        return "There is an error in getting the correct time\n";
    }
    time_t bootSec = bootTime.tv_sec;
    time_t currentTime = time(nullptr);
    if (currentTime == (time_t)-1) {
        return "Can't get current time\n";
    }
    long timeInSec = (long)difftime(currentTime, bootSec);
    long days = timeInSec / SEC_IN_DAY;
    long hours = (timeInSec % SEC_IN_DAY) / SEC_IN_HOUR;
    long minutes = (timeInSec % SEC_IN_HOUR)/ SEC_IN_MINUTE;
    std::string finalString = std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    return finalString;
}

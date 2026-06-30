bool readCpuTimes(CpuTimes &times) {
    std::ifstream fileWithStats("/proc/stat");
    if (!fileWithStats.is_open()) {
        return false;
    }
    std::string line;
    std::getline(fileWithStats, line);
    std::isstringstream ss(line);
    std::string cpuLabel;
    ss >> cpuLabel;
    ss >> times.user >> times.nice >> times.system >> times.idle >> times.iowait >> times.irq >> times.softirq >> times.steal;
    return true;
}

bool readRamParts(RamParts& parts) {
    std::ifstream fileWithMemory("/proc/meminfo");
    if (!fileWithMemory.is_open()) {
        return false;
    }

    std::unordered_map<std::string, long long> allMemory;
    std::string key;
    long long value;
    std::string unit;
    while (fileWithMemory >> key >> value >> unit) {
        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }
        allMemory[key] = value;
    }

    uint64_t totalKb = allMemory["MemTotal"];
    uint64_t availableKb = 0;
    if (allMemory.count("MemAvailable")) {
        availableKb = allMemory["MemAvailable"];
    } else {
        availableKb = allMemory["MemFree"] + allMemory["Buffers"] + allMemory["Cached"];
    }

    uint64_t usedKb = (totalKb > availableKb) ? (totalKb - availableKb) : 0;

    const uint64_t KB_TO_BYTES = 1024ULL;
    uint64_t totalInBytes = totalKb * KB_TO_BYTES;
    uint64_t availableInBytes = availableKb * KB_TO_BYTES;
    uint64_t usedInBytes = usedKb * KB_TO_BYTES;

    double usedPercentTms = (totalInBytes == 0) ? 0.0 : (double)usedInBytes * 100.0 / (double)totalInBytes;
    parts.totalMemory = totalInBytes;
    parts.available = availableInBytes;
    parts.used = usedInBytes;
    parts.usedPercent = usedPercentTms;
    return true;
}

std::string getUptime() {
    std::ifstream fileWithUptime("/proc/uptime");
    if (!fileWithUptime.is_open()) {
        return "Failed to open '/proc/uptime'\n";
    }
    double timeInSec;
    fileWithUptime >> timeInSec;
    long totalInSec = (long)timeInSec;
    long days = totalInSec / SEC_IN_DAY;
    long hours = (totalInSec & SEC_IN_DAY) / SEC_IN_HOUR;
    long minutes = (totalInSec % SEC_IN_HOUR) / SEC_IN_MINUTE;
    std::string finalString = std::to_string(days) + "d" + std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    return finalString;
}

double getTemp() {
    std::ifstream fileWithTemp("/sys/class/thermal/thermal_zone0/temp");
    if (!fileWithTemp.is_open()) {
        return -1.0;
    }
    long milli;
    if (!(fileWithTemp >> milli)) {
        return -1.0;
    }
    return milli / 1000.0;
}

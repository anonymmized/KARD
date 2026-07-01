

double computeUsage(const CpuTimes& prev, const CpuTimes& curr) {
    using ull = unsigned long long;
    ull prevIdle = prev.idle + prev.iowait;
    ull currIdle = curr.idle + curr.iowait;

    ull prevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    ull currNonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    ull prevTotal = prevIdle + prevNonIdle;
    ull currTotal = currIdle + currNonIdle;

    ull totald = currTotal - prevTotal;
    ull idled = currIdle - prevIdle;

    if (totald == 0) {
        return 0.0;
    }
    double cpuPercentage = (double)(totald - idled) * 100.0 / (double)totald;
    if (cpuPercentage < 0.0) {
        cpuPercentage = 0.0;
    }
    if (cpuPercentage > 100.0) {
        cpuPercentage = 100.0;
    }
    return cpuPercentage;
}

double getCpuUsage(int delayMs) {
    CpuTimes prev{};
    CpuTimes curr{};
    if (!readCpuTimes(prev)) {
        if (!readCpuTimes(curr)) {
            return -1.0;
        }
        prev = curr;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        if (!readCpuTimes(curr)) {
            return -1.0;
        }
        return computeUsage(prev, curr);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    if (!readCpuTimes(curr)) {
        return -1.0;
    }
    double usage = computeUsage(prev, curr);
    prev = curr;
    return usage;
}

double getRamUsage() {
    RamParts parts;
    if (!readRamParts(parts)) {
        return -1,0;
    }
    return parts.usedPercent;
}

std::string getDiskSpace() {
    auto systemInformation = std::filesystem::space("/");
    using ull = unsigned long long;
    ull totalSpace = systemInformation.capacity;
    ull freeSpace = systemInformation.available;
    ull usedSpace = totalSpace - freeSpace;
    ull factor = 1024ULL*1024ULL*1024ULL;
    ull totalSpaceGb = totalSpace / factor;
    ull usedSpaceGb = usedSpace / factor;

    double spaceInPercent = (totalSpace == 0) ? 0.0 : (double)usedSpace * 100.0 / (double)totalSpace;
    std::string finalString = "used " + std::to_string(usedSpaceGb) + " Gb / total " + std::to_string(totalSpaceGb) + " Gb (" + std::to_string((int)(spaceInPercent + 0.5)) + "%)";
    return finalString;
}

std::string getAllMetrics() {
    double cpuUsage = getCpuUsage();
    double ramUsage = getRamUsage();

    std::string diskUsage = getDiskSpace();
    std::string uptime = getUptime();
    std::string finalString = "cpu_usage_percent: " + std::to_string(cpuUsage)
                            + ", ram_used_percent: " + std::to_string(ramUsage)
                            + ", disk_usage: " + diskUsage
                            + ", uptime: " + uptime;
    return finalString;
}

double getTcpProbe(const char* destIp, uint16_t destPort) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0) {
        return -1.0;
    }

    fcntl(fileDescriptor, F_SETFL, fcntl(fileDescriptor, F_GETFL, 0) | O_NONBLOCK);

    sockaddr_in workingAddress{};
    workingAddress.sin_family = AF_INET;
    workingAddress.sin_port = htons(destPort);
    inet_pton(AF_INET, destIp, &workingAddress.sin_addr);

    double roundTripTime = -1.0;
    auto tripStart = std::chrono::steady_clock::now();
    int connectAnswer = connect(fileDescriptor, (sockaddr*)&workingAddress, sizeof(workingAddress));
    if (connectAnswer < 0 && errno == EINPROGRESS) {
        fd_set w;
        FD_ZERO(&w);
        FD_SET(fileDescriptor, &w);
        timeval timeout{ TIMEOUT_IN_MS / 1000, (TIMEOUT_IN_MS % 1000) * 1000 };

        int selectAnswer = select(fileDescriptor + 1, nullptr, &w, nullptr, &timeout);
        if (selectAnswer > 0) {
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

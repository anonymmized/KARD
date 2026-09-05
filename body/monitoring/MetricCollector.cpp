#include "body/monitoring/MetricCollector.hpp"

#include "body/monitoring/dockerMetrics.hpp"
#include "body/monitoring/monitor.hpp"

#include <iomanip>
#include <sstream>
#include <string>

namespace {
constexpr double CPU_WARNING_PERCENT = 70.0;
constexpr double CPU_CRITICAL_PERCENT = 90.0;
constexpr double RAM_WARNING_PERCENT = 75.0;
constexpr double RAM_CRITICAL_PERCENT = 90.0;
constexpr double DISK_WARNING_PERCENT = 80.0;
constexpr double DISK_CRITICAL_PERCENT = 90.0;
constexpr double TEMP_WARNING_CELSIUS = 75.0;
constexpr double TEMP_CRITICAL_CELSIUS = 90.0;

MetricState stateForValue(double value, double warning, double critical) {
    if (value < 0.0) {
        return MetricState::Unavailable;
    }
    if (value >= critical) {
        return MetricState::Critical;
    }
    if (value >= warning) {
        return MetricState::Warning;
    }
    return MetricState::Normal;
}

std::string formatNumber(double value) {
    if (value < 0.0) {
        return "unavailable";
    }

    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << value;
    return output.str();
}

double diskUsagePercent(const std::string& value) {
    const std::size_t openParen = value.rfind('(');
    const std::size_t percent = value.find('%', openParen);
    if (openParen == std::string::npos || percent == std::string::npos) {
        return -1.0;
    }

    try {
        return std::stod(value.substr(openParen + 1, percent - openParen - 1));
    } catch (const std::exception&) {
        return -1.0;
    }
}
} // namespace

std::vector<Metric> MetricCollector::collectCpu() const {
    const double value = getCpuUsage();
    Metric cpu = {"get_cpu", formatNumber(value), "%",
                  stateForValue(value, CPU_WARNING_PERCENT, CPU_CRITICAL_PERCENT)};
    return {cpu};
}

std::vector<Metric> MetricCollector::collectRam() const {
    const double value = getRamUsage();
    Metric ram = {"get_ram", formatNumber(value), "%",
                  stateForValue(value, RAM_WARNING_PERCENT, RAM_CRITICAL_PERCENT)};
    return {ram};
}

std::vector<Metric> MetricCollector::collectDisk() const {
    const std::string value = getDiskSpace();
    const double percent = diskUsagePercent(value);
    Metric disk = {"get_disk", value, "",
                   stateForValue(percent, DISK_WARNING_PERCENT, DISK_CRITICAL_PERCENT)};
    return {disk};
}

std::vector<Metric> MetricCollector::collectUptime() const {
    const std::string value = getUptime();
    const bool failed = value.find("error") != std::string::npos ||
                        value.find("Can't") != std::string::npos ||
                        value.find("Failed") != std::string::npos;
    Metric uptime = {"get_uptime", value, "",
                     failed ? MetricState::Unavailable : MetricState::Normal};
    return {uptime};
}

std::vector<Metric> MetricCollector::collectTemp() const {
    const double value = getTemp();
    Metric temp = {"get_temp", formatNumber(value), value < 0.0 ? "" : "C",
                   stateForValue(value, TEMP_WARNING_CELSIUS, TEMP_CRITICAL_CELSIUS)};
    return {temp};
}

std::vector<Metric> MetricCollector::collectDockerStatus() const {
    const std::string rawValue = checkIfDockerIsRunning();
    const bool running = rawValue.find("true") != std::string::npos;
    Metric dockerStatus = {"get_docker_status", running ? "running" : "not running", "",
                           running ? MetricState::Normal : MetricState::Unavailable};
    return {dockerStatus};
}

std::vector<Metric> MetricCollector::collectDockerRunningState() const {
    const std::string rawValue = getNumOfRunningContainers();
    const std::size_t separator = rawValue.rfind(':');
    const std::string value = separator == std::string::npos
                                  ? rawValue
                                  : rawValue.substr(separator + 2);
    const bool unavailable = value == "-1";
    Metric dockerRunningState = {"get_docker_running",
                                 unavailable ? "unavailable" : value,
                                 "",
                                 unavailable ? MetricState::Unavailable : MetricState::Normal};
    return {dockerRunningState};
}

std::vector<Metric> MetricCollector::collectDockerList() const {
    const std::string value = getContainersList();
    Metric dockerList = {"get_docker_list", value, "", MetricState::Normal};
    return {dockerList};
}

std::vector<Metric> MetricCollector::collectAll() const {
    std::vector<Metric> allMetrics = {};
    auto append = [&allMetrics](const std::vector<Metric>& metrics) {
        allMetrics.insert(allMetrics.end(), metrics.begin(), metrics.end());
    };
    append(collectCpu());
    append(collectRam());
    append(collectDisk());
    append(collectUptime());
    append(collectTemp());
    append(collectDockerStatus());
    append(collectDockerRunningState());
    append(collectDockerList());
    return allMetrics;
}






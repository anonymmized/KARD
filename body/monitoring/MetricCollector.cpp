#include "body/monitoring/MetricCollector.hpp"

#include "body/monitoring/dockerMetrics.hpp"
#include "body/monitoring/monitor.hpp"

std::vector<Metric> MetricCollector::collectCpu() const {
    Metric cpu = {"get_cpu", std::to_string(getCpuUsage()), "%"};
    return {cpu};
}

std::vector<Metric> MetricCollector::collectRam() const {
    Metric ram = {"get_ram", std::to_string(getRamUsage()), "%"};
    return {ram};
}

std::vector<Metric> MetricCollector::collectDisk() const {
    Metric disk = {"get_disk", getDiskSpace(), ""};
    return {disk};
}

std::vector<Metric> MetricCollector::collectUptime() const {
    Metric uptime = {"get_uptime", getUptime(), ""};
    return {uptime};
}

std::vector<Metric> MetricCollector::collectTemp() const {
    Metric temp = {"get_temp", std::to_string(getTemp()), "celsius"};
    return {temp};
}

std::vector<Metric> MetricCollector::collectDockerStatus() const {
    Metric dockerStatus = {"get_docker_status", checkIfDockerIsRunning(), ""};
    return {dockerStatus};
}

std::vector<Metric> MetricCollector::collectDockerRunningState() const {
    Metric dockerRunningState = {"get_docker_running", getNumOfRunningContainers(), ""};
    return {dockerRunningState};
}

std::vector<Metric> MetricCollector::collectDockerList() const {
    Metric dockerList = {"get_docker_list", getContainersList(), ""};
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







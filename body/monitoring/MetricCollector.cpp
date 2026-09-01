#include "brain/MetricController.hpp"

#include "body/monitoring/dockerMetrics.hpp"
#include "body/monitoring/monitor.hpp"

Metric Collector::collectCpu() const {
    Metric cpu = {"get_cpu", std::to_string(getCpuUsege()), "%"};
    return cpu;
}

Metric Collector::collectRam() const {
    Metric ram = {"get_ram", std::to_string(getRamUsage()), "%"};
    return ram;
}

Metric Collector::collectDisk() const {
    Metric disk = {"get_disk", getDiskSpace(), ""};
    return disk;
}

Metric Collector::collectUptime() const {
    Metric uptime = {"get_uptime", getUptime(), ""};
    return uptime;
}

Metric Collector::collectTemp() const {
    Metric temp = {"get_temp", std::to_string(getTemp()), "celsius"};
    return temp;
}

Metric Collector::collectDockerStatus() const {
    Metric dockerStatus = {"get_docker_status", checkIfDockerIsRunning(), ""};
    return dockerStatus;
}

Metric Collector::collectDockerRunningState() const {
    Metric dockerRunningState = {"get_docker_running", getNumOfRunningContainers(), ""};
    return dockerRunningState;
}

Metric Collector::collectDockerList() const {
    Metric dockerList = {"get_docker_list", getContainersList(), ""};
    return dockerList;
}

std::vector<Metric> Collector::collectAll() const {
    std::vector<Metric> allMetrics = {};
    allMetrics.push_back(collectCpu());
    allMetrics.push_back(collectRam());
    allMetrics.push_back(collectDisk());
    allMetrics.push_back(collectUptime());
    allMetrics.push_back(collectTemp());
    allMetrics.push_back(collectDockerStatus());
    allMetrics.push_back(collectDockerRunningState());
    allMetrics.push_back(collectDockerList());
    return allMetrics;
}







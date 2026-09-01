#pragma once

#include "contracts/BrainAnswer.hpp"
#include <vector>

class MetricCollector {
    public:
        std::vector<Metric> collectCpu() const;
        std::vector<Metric> collectRam() const;
        std::vector<Metric> collectDisk() const;
        std::vector<Metric> collectUptime() const;
        std::vector<Metric> collectTemp() const;
        std::vector<Metric> collectDockerStatus() const;
        std::vector<Metric> collectDockerRunningState() const;
        std::vector<Metric> collectDockerList() const;
        std::vector<Metric> collectAll() const;
};

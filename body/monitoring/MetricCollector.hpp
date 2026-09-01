#pragma once

#include "contracts/BrainAnswer.hpp"
#include <vector>

class Collector {
    public:
        Metric collectCpu() const;
        Metric collectRam() const;
        Metric collectDisk() const;
        Metric collectUptime() const;
        Metric collectTemp() const;
        Metric collectDockerStatus() const;
        Metric collectDockerRunningState() const;
        Metric collectDockerList() const;
        std::vector<Metric> collectAll() const;
};

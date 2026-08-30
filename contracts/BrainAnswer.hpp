#pragma once 

#include <string>

struct BrainAnswer {
    std::string plainAnswer;
    std::string cpuUsage;
    std::string ramUsage;
    std::string diskSpace;
    std::string uptime;
    std::string temp;
    std::string dockerStatus;
    std::string dockerIsRunning;
    std::string dockerList;
};

#pragma once 

#include <string>
#include <vector>

struct Metric {
    std::string name;
    std::string value;
    std::string unit;
};

struct BrainAnswer {
    std::string textAnswer;
    std::vector<Metric> metrics;
};

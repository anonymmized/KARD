#pragma once 

#include <string>
#include <vector>

enum class MetricState {
    Normal,
    Warning,
    Critical,
    Unavailable
};

inline const char* metricStateToString(MetricState state) {
    switch (state) {
        case MetricState::Normal:
            return "normal";
        case MetricState::Warning:
            return "warning";
        case MetricState::Critical:
            return "critical";
        case MetricState::Unavailable:
            return "unavailable";
    }
    return "unavailable";
}

struct Metric {
    std::string name;
    std::string value;
    std::string unit;
    MetricState state;
};

struct BrainAnswer {
    std::string textAnswer;
    std::vector<Metric> metrics;
};

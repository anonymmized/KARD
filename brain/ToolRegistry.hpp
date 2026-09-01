#pragma once

#include "contracts/BrainAnswer.hpp"
#include "body/monitoring/MetricCollector.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr const char* IP_TO_PING = "1.1.1.1";
constexpr int HTTPS_PORT = 443;

inline const std::string TOOL_NOT_EXIST = "There is no tool like this.";

struct ToolResult {
    std::vector<Metric> metrics;
    std::string text;
};

class ToolRegistry {
    private:
        MetricCollector metricCollector;
        int parsePeriod(const std::string& period);
        std::vector<Metric> executeMetricTool(const std::string& toolName);
        void saveSnapshots(const std::vector<Metric>& metrics);
    public:
        ToolResult runToolCall(const nlohmann::json& call);
        void removeUselessObjects(std::vector<nlohmann::json>& base);
};

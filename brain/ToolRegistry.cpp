#include "brain/ToolRegistry.hpp"

#include "body/monitoring/dockerMetrics.hpp"
#include "body/monitoring/monitor.hpp"
#include "body/snapshot_log.hpp"

#include "../common/timeConstants.hpp"

#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
    using CollectorMethod = std::vector<Metric> (MetricCollector::*)() const;
    struct ToolHandler {
        CollectorMethod collectorMethod;
        bool loadSnapshot;
};

static const std::unordered_map<std::string, ToolHandler> handlers = {
    {"get_cpu", {&MetricCollector::collectCpu, true}},
    {"get_ram", {&MetricCollector::collectRam, true}},
    {"get_disk", {&MetricCollector::collectDisk, true}},
    {"get_uptime", {&MetricCollector::collectUptime, true}},
    {"get_all", {&MetricCollector::collectAll, false}},
    {"get_temp", {&MetricCollector::collectTemp, true}},
    {"get_docker_status", {&MetricCollector::collectDockerStatus, false}},
    {"get_docker_running", {&MetricCollector::collectDockerRunningState, false}},
    {"get_docker_list", {&MetricCollector::collectDockerList, false}}};
} //namespace

std::vector<Metric> ToolRegistry::executeMetricTool(const std::string &toolName) {
    const ToolHandler& handler = handlers.at(toolName);
    std::vector<Metric> metrics = (metricCollector.*handler.collectorMethod)();

    if (handler.loadSnapshot) {
        saveSnapshots(metrics);
    }
    return metrics;
}

void ToolRegistry::saveSnapshots(const std::vector<Metric>& metrics) {
    for (const Metric& metric : metrics) {
        appendSnapshot(metric.name, metric.value);
    }
}

int ToolRegistry::parsePeriod(const std::string &period) {
  if (period.empty()) {
    return SEC_IN_HOUR;
  }
  int timeNumber;
  try {
    timeNumber = std::stoi(period);
  } catch (const std::exception &) {
    return SEC_IN_HOUR;
  }
  if (timeNumber <= 0) {
    return SEC_IN_HOUR;
  }
  char unit = period.back();
  if (unit == 'd')
    return timeNumber * HOURS_IN_DAY * SEC_IN_HOUR;
  if (unit == 'm')
    return timeNumber * SEC_IN_MINUTE;
  if (unit == 'h')
    return timeNumber * SEC_IN_HOUR;
  return timeNumber;
}

ToolResult ToolRegistry::runToolCall(const nlohmann::json &call) {
    std::string toolName = call["function"]["name"].get<std::string>();
    if (toolName == "summarize_health") {
        nlohmann::json arguments = call["function"]["arguments"];
        if (arguments.is_string()) {
            arguments = nlohmann::json::parse(arguments.get<std::string>());
        }
        const std::string period = arguments.value("period", "1h");
        const int periodSeconds = parsePeriod(period);
        return ToolResult{{}, summarizeHealth(periodSeconds)};
    } 
    if (toolName == "get_network") {
        const double networkAnswerTime = getTcpProbe(IP_TO_PING, HTTPS_PORT);
        if (networkAnswerTime < 0) {
            return ToolResult{{}, "network unreachable"};
        }

        Metric networkMetrics{"get_network", std::to_string(networkAnswerTime), "ms"};

        appendSnapshot("get_network", std::to_string(networkAnswerTime));
        return ToolResult{{networkMetrics}, ""};
    } 
    if (handlers.find(toolName) == handlers.end()){
        return ToolResult{{}, TOOL_NOT_EXIST};
    }
    return ToolResult{executeMetricTool(toolName), ""};
}

void ToolRegistry::removeUselessObjects(std::vector<nlohmann::json> &base) {
    while (base.size() > 20) {
        base.erase(base.begin());
    }

    while (!base.empty() && base.front()["role"] != "user") {
        base.erase(base.begin());
    }
}


#include "brain/ToolRegistry.hpp"

#include "body/monitoring/monitor.hpp"
#include "body/snapshot_log.hpp"

#include <string>
#include <nlohmann::json>

namespace {
    struct ToolHandler {
        std::function<std::string()> functionToRun;
        std::string prefix;
        bool loadSnapshot;
    };

    static const std::unordered_map<std::string, ToolHandler> handlers = {
        {"get_cpu", {[]{ return std::to_string(getCpuUsage()); }, "cpu_usage_percent: ", true}},
        {"get_ram", {[]{ return std::to_string(getRamUsage()); }, "ram_used_percent: ", true}},
        {"get_disk", {[]{ return getDiskSpace(); }, "", true}},
        {"get_uptime", {[]{ return getUptime(); }, "uptime: ", true}},
        {"get_all", {[]{ return getAllMetrics(); }, "", false}},
        {"get_temp", {[]{ return std::to_string(getTemp()); }, "temperature_celsius: ", true}},
        {"get_docker_status", {[]{ return checkIfDockerIsRunning(); }, "", false}},
        {"get_docker_running", {[]{ return getNumOfRunningContainers(); }, "", false}},
        {"get_docker_list", {[]{ return getContainersList(); }, "", false}}
    };
}

int parsePeriod(const std::string& period) {
    if (period.empty()) {
        return 3600;
    }
    int timeNumber;
    try {
        timeNumber = std::stoi(period);
    } catch (const std::exception&) {
        return 3600;
    }
    if (timeNumber <= 0) {
        return 3600;
    }
    char unit = period.back();
    if (unit == 'd') return timeNumber * 24 * 3600;
    if (unit == 'm') return timeNumber * 60;
    if (unit == 'h') return timeNumber * 3600;
    return timeNumber;
}

std::string pushAllContent(const std::string& toolName) {
    auto toolInMap = handlers.find(toolName);
    if (toolName == handlers.end()) {
        return TOOL_NOT_EXIST;
    }

    std::string result = toolInMap->second.functionToRun();
    if (toolInMap->second.loadSnapshot) {
        appendSnapshot(toolName, result);
    }
    return toolInMap->second.prefix + result;
}

std::string runToolCall(const nlohmann::json& call) {
    std::string answerToPush;
    std::string toolName = call["function"]["name"];
    if (toolName == "summarize_health") {
        auto arguments = call["function"]["arguments"];
        if (arguments.is_string()) {
            arguments = nlohmann::json::parse(arguments.get<std::string>());
        }
        std::string period = arguments.value("period", "1h");
        int hours = parsePeriod(period);
        answerToPush = summarizeHealth(hours);
    } else if (toolName == "get_network") {
        double networkAnswerTime = getTcpProbe(IP_TO_PING, HTTPS_PORT);
        if (networkAnswerTime >= 0) {
            appendSnapshot("get_network", std::to_string(networkAnswerTime));
        }
        answerToPush = (networkAnswerTime < 0) ? "network unreachable" : "network reachable, rtt_ms: " + std::to_string(networkAnswerTime);
    } else {
        answerToPush = pushAllContent(toolName);
    }
    return answerToPush;
}


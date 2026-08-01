#include "OllamaBrain.hpp"
#include "body/monitoring/monitor.hpp"
#include "body/snapshot_log.hpp"

#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <functional>

namespace {
    struct ToolHandler {
        std::function<std::string()> functionToRun;
        std::string prefix;
        bool loadToSnapshot;
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

nlohmann::json OllamaBrain::uploadConfig() {
    std::ifstream in(CONFIG_PATH);
    nlohmann::json config;
    if (!in.is_open()) {
        std::cerr << "Unable to open config file\n";
        return config;
    }
    in >> config;
    in.close();
    return config;
}

int OllamaBrain::parsePeriod(const std::string& p) {
    if (p.empty()) return 3600;
    int num;
    try {
        num = std::stoi(p);
    } catch (const std::exception&) {
        return 3600;
    }
    if (num <= 0) return 3600;
    char unit = p.back();
    if (unit == 'd') return num * 24 * 3600;
    if (unit == 'm') return num * 60;
    if (unit == 'h') return num * 3600;
    return num;
}

nlohmann::json OllamaBrain::collectAllMessages(const std::string& systemPrompt) {
    nlohmann::json allMessages = nlohmann::json::array();
    allMessages.push_back({{"role", "system"}, {"content", systemPrompt}});

    for (const auto& part : base) {
        allMessages.push_back(part);
    }
    return allMessages;
}

nlohmann::json OllamaBrain::getBody() {
    nlohmann::json config = uploadConfig();
    url = config["url"];
    nlohmann::json allMessages = collectAllMessages(config["system_prompt"]);
    config.push_back({"messages", allMessages});
    return config;
}

void OllamaBrain::pushToolContent(const std::string& content) {
    base.push_back({{"role", "tool"}, {"content", content}});
}

void OllamaBrain::pushAllContent(const std::string& toolName) {
    auto toolInMap = handlers.find(toolName);
    if (toolInMap == handlers.end()) {
        pushToolContent(TOOL_NOT_EXIST);
        return;
    }

    std::string result = toolInMap->second.functionToRun();
    if (toolInMap->second.loadToSnapshot) {
        appendSnapshot(toolName, result);
    }
    pushToolContent(toolInMap->second.prefix + result);
}

std::string OllamaBrain::ask(const std::string& request) {
    base.push_back({{"role","user"},{"content",request}});
    int remainingIterations = MAX_TOOL_ITERATIONS;
    std::string stringReply;
    while (remainingIterations != 0) {
        nlohmann::json body = getBody();

        cpr::Response resp = cpr::Post(
            cpr::Url(url),
            cpr::Body{body.dump()},
            cpr::Header{{"Content-Type","application/json"}}
        );

        if (resp.status_code != 200) {
            return "It seems an error)\n";
        }

        auto reply = nlohmann::json::parse(resp.text);
        base.push_back(reply["message"]);
        if (!reply["message"].contains("tool_calls")) {
            return reply["message"]["content"].get<std::string>();
        }

        for (const auto& call : reply["message"]["tool_calls"]) {
            std::string tool_name = call["function"]["name"];
            if (tool_name == "summarize_health") {
                auto args = call["function"]["arguments"];
                if (args.is_string()) {
                    args = nlohmann::json::parse(args.get<std::string>());
                }
                std::string period = args.value("period", "1h");
                int hours = parsePeriod(period);
                std::string value = summarizeHealth(hours);
                pushToolContent(value);
            } else if (tool_name == "get_network") {
                double networkAnswerTime = getTcpProbe(IP_TO_PING, HTTPS_PORT);
                if (networkAnswerTime >= 0) {
                    appendSnapshot("get_network", std::to_string(networkAnswerTime));
                }
                std::string answerToPush = (networkAnswerTime < 0) ? "network unreachable" : "network reachable, rtt_ms: " + std::to_string(networkAnswerTime); // TODO: change variable name
                pushToolContent(answerToPush);
            } else {
                pushAllContent(tool_name);
            }
        }

        while (base.size() > 20) {
            base.erase(base.begin());
        }

        while (!base.empty() && base.front()["role"] != "user") {
            base.erase(base.begin());
        }
        stringReply = reply["message"]["content"].get<std::string>();
        remainingIterations--;
    }
    return stringReply;
}



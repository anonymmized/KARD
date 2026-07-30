#include "OllamaBrain.hpp"
#include "body/monitoring/monitor.hpp"
#include "body/snapshot_log.hpp"

#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <chrono>

nlohmann::json OllamaBrain::uploadConfig() {
    std::ifstream in(CONFIG_PATH);
    if (!in.is_open()) {
        std::cerr << "Unable to open config file\n";
        return;
    }
    nlohmann::json config;
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
    base.push_back({"role", "tool"}, {"content", content});
}

std::string OllamaBrain::ask(const std::string& request) {
    base.push_back({{"role","user"},{"content",request}});
    int remainingIterations = MAX_TOOL_ITERATIONS;
    std::string str_reply;
    while (remainingIterations != 0) {
        nlohmann::json body = getBody(request);

        cpr::Response resp = cpr::Post(
            cpr::Url(url),
            cpr::Body{body.dump()},
            cpr::Header{{"Content-Type","application/json"}}
        );

        if (resp.status_code != 200) return "It seems an error)\n";
        auto reply = nlohmann::json::parse(resp.text);
        base.push_back(reply["message"]);
        if (!reply["message"].contains("tool_calls")) return reply["message"]["content"].get<std::string>();
        for (const auto& call : reply["message"]["tool_calls"]) {
            std::string tool_name = call["function"]["name"]; // TODO: do a map string-ToolHandler              ToolHandler contains function to run; prefix; add to snapshot or not
            if (tool_name == "get_cpu") {
                std::string value = std::to_string(getCpuUsage());
                appendSnapshot("get_cpu", vvalue);
                pushToolContent("cpu_usage_percent: " + value);
            } else if (tool_name == "get_ram") {
                std::string value = std::to_string(getRamUsage());
                appendSnapshot("get_ram", value);
                pushToolContent("ram_used_percent: " + value);
            } else if (tool_name == "get_disk") {
                std::string value = getDiskSpace();
                appendSnapshot("get_disk", value);
                pushToolContent(value)
            } else if (tool_name == "get_uptime") {
                std::string value = getUptime();
                appendSnapshot("get_uptime", value);
                pushToolContent("uptime: " + value);
            } else if (tool_name == "get_all") {
                std::string value = getAllMetrics();
                pushToolContent(value);
            } else if (tool_name == "get_temp") {
                std::string currentTempText = std::to_string(getTemp());
                appendSnapshot("get_temp", currentTempText);
                pushToolContent("temperature_celsius: " + currentTempText);
            } else if (tool_name == "summarize_health") {
                auto args = call["function"]["arguments"];
                if (args.is_string()) {
                    args = nlohmann::json::parse(args.get<std::string>());
                }
                std::string period = args.value("period", "1h");
                int hours = parsePeriod(period);
                std::string value = summarizeHealth(hours);
                pushToolContent(value);
            } else if (tool_name == "get_network") {
                double networkAnswerTime = getTcpProbe("1.1.1.1", 443);
                if (networkAnswerTime >= 0) {
                    appendSnapshot("get_network", std::to_string(networkAnswerTime));
                }
                std::string answerToPush = (networkAnswerTime < 0) ? "network unreachable" : "network reachable, rtt_ms: " + std::to_string(networkAnswerTime); // TODO: change variable name
                pushToolContent("rtt_ms: " + answerToPush);
            } else if (tool_name == "get_docker_status") {
                pushToolContent(checkIfDockerIsRunning()); // TODO: dont run docker function from another one
            } else if (tool_name == "get_docker_running") {
                pushToolContent(getNumOfRunningContainers()); // TODO: create string vars for each function in 'if'
            } else if (tool_name == "get_docker_list") {
                pushToolContent(getContainersList());
            }
            else {
                pushToolContent(TOOL_NOT_EXIST);
            }
        }
        while (base.size() > 20) base.erase(base.begin());
        while (!base.empty() && base.front()["role"] != "user") base.erase(base.begin());
        str_reply = reply["message"]["content"].get<std::string>();
        remainingIterations--;
    }
    return str_reply;
}



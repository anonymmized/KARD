#include "OllamaBrain.hpp"
#include "body/monitoring/monitor.hpp"
#include "body/snapshot_log.hpp"
#include "brain/ToolRegistry.hpp"

#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <functional>

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

nlohmann::json OllamaBrain::collectAllMessages(const std::string& systemPrompt) {
    nlohmann::json allMessages = nlohmann::json::array();
    allMessages.push_back({{"role", "system"}, {"content", systemPrompt}});

    for (const auto& part : base) {
        allMessages.push_back(part);
    }
    return allMessages;
}

nlohmann::json OllamaBrain::getBody() {
    nlohmann::json body = config;
    body["messages"] = collectAllMessages(config["system_prompt"]);
    return body;
}

void OllamaBrain::pushToolContent(const std::string& content) {
    base.push_back({{"role", "tool"}, {"content", content}});
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
            pushToolContent(runToolCall(call));
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



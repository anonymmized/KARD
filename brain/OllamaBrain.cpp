#include "OllamaBrain.hpp"
#include "body/systemMonitor.hpp"

#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

void OllamaBrain::uploadConfig() {
    std::ifstream in(CONFIG_PATH);
    if (!in.is_open()) {
        std::cerr << "Unable to open config file\n";
        return;
    }
    in >> json_config;
    in.close();
}

nlohmann::json OllamaBrain::getBody(const std::string& request) {
    nlohmann::json messages = nlohmann::json::array();
    uploadConfig();
    url = json_config["url"];
    messages.push_back({{"role","system"},{"content",json_config["system_prompt"]}});
    for (const auto& m : base) messages.push_back(m);
    json_config.push_back({"messages",messages});
    return json_config;
}

std::string OllamaBrain::ask(const std::string& request) {
    base.push_back({{"role","user"},{"content",request}});
    int i = 5;
    std::string str_reply;
    while (i != 0) {
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
        std::string tool_name = reply["message"]["tool_calls"][0]["function"]["name"];
        if (tool_name == "get_cpu") {
            base.push_back({{"role","tool"},{"content", std::to_string(getCpuUsage())}});
        } else {
            base.push_back({{"role","tool"},{"content", "There is no tool like this."}});
        }
        while (base.size() > 20) base.erase(base.begin());
        str_reply = reply["message"]["content"].get<std::string>();
        i--;
    }
    return str_reply;
}



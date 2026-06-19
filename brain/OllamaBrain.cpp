#include "OllamaBrain.hpp"
#include "body/systemMonitor.hpp"
#include "body/snapshot_log.hpp"

#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
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
        for (const auto& call : reply["message"]["tool_calls"]) {
            std::string tool_name = call["function"]["name"];
            if (tool_name == "get_cpu") {
                std::string v = std::to_string(getCpuUsage());
                appendSnapshot("get_cpu", v);
                base.push_back({{"role","tool"},{"content", v}});
            } else if (tool_name == "get_ram") {
                std::string v = std::to_string(getRamUsage());
                appendSnapshot("get_ram", v);
                base.push_back({{"role","tool"},{"content", v}});
            } else if (tool_name == "get_disk") {
                std::string v = getDiskSpace();
                appendSnapshot("get_disk", v);
                base.push_back({{"role","tool"},{"content", v}});
            } else if (tool_name == "get_uptime") {
                std::string v = getUptime();
                appendSnapshot("get_uptime", v);
                base.push_back({{"role","tool"},{"content", v}});
            } else if (tool_name == "get_all") {
                std::string v = getAll();
                //sendToLog("get_all", v);
                base.push_back({{"role","tool"},{"content", v}});
            } else if (tool_name == "get_temp") {
                std::string v = std::to_string(getTemp());
                appendSnapshot("get_temp", v);
                base.push_back({{"role","tool"},{"content", v}});
            }
            else if (tool_name == "summarize_health") {
                auto args = call["function"]["arguments"];
                if (args.is_string()) args = nlohmann::json::parse(args.get<std::string>());

                std::string period = args.value("period", "1h");
                int hours = parsePeriod(period);
                std::string v = summarizeHealth(hours);
                base.push_back({{"role","tool"},{"content",v}});
            }
            else {
                base.push_back({{"role","tool"},{"content", "There is no tool like this."}});
            }
        }
        while (base.size() > 20) base.erase(base.begin());
        while (!base.empty() && base.front()["role"] != "user") base.erase(base.begin());
        str_reply = reply["message"]["content"].get<std::string>();
        i--;
    }
    return str_reply;
}



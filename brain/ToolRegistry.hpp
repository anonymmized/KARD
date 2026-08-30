#pragma once

#include "ModelAnswer.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr const char* IP_TO_PING = "1.1.1.1";
constexpr int HTTPS_PORT = 443;

inline const std::string TOOL_NOT_EXIST = "There is no tool like this.";


class ToolRegistry {
    private:
        std::unordered_map<std::string, std::string*> fields;
        ModelAnswer* modelAnswer;
        int parsePeriod(const std::string& period);
        void updateModelAnswer(const std::string& toolName, const std::string& toolState);
    public:
        ToolRegistry(ModelAnswer* _modelAnswer) : modelAnswer(_modelAnswer) {
            if (modelAnswer == nullptr) {
                throw std::invalid_argument("ModelAnswer is null");
            }
            fields = {
                {"get_cpu", &modelAnswer->cpuUsage},
                {"get_ram", &modelAnswer->ramUsage},
                {"get_disk", &modelAnswer->diskSpace},
                {"get_uptime", &modelAnswer->uptime},
                {"get_temp", &modelAnswer->temp},
                {"get_docker_status", &modelAnswer->dockerStatus},
                {"get_docker_running", &modelAnswer->dockerIsRunning},
                {"get_docker_list", &modelAnswer->dockerList}
            };
        }
        std::string runToolCall(const nlohmann::json& call);
        void removeUselessObjects(std::vector<nlohmann::json>& base);
};

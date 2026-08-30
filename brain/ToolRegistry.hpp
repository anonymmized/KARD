#pragma once

#include "contracts/BrainAnswer.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr const char* IP_TO_PING = "1.1.1.1";
constexpr int HTTPS_PORT = 443;

inline const std::string TOOL_NOT_EXIST = "There is no tool like this.";


class ToolRegistry {
    private:
        std::unordered_map<std::string, std::string*> fields;
        BrainAnswer* brainAnswer;
        int parsePeriod(const std::string& period);
        void updateModelAnswer(const std::string& toolName, const std::string& toolState);
    public:
        ToolRegistry(BrainAnswer* _brainAnswer) : brainAnswer(_brainAnswer) {
            if (brainAnswer == nullptr) {
                throw std::invalid_argument("ModelAnswer is null");
            }
            fields = {
                {"get_cpu", &brainAnswer->cpuUsage},
                {"get_ram", &brainAnswer->ramUsage},
                {"get_disk", &brainAnswer->diskSpace},
                {"get_uptime", &brainAnswer->uptime},
                {"get_temp", &brainAnswer->temp},
                {"get_docker_status", &brainAnswer->dockerStatus},
                {"get_docker_running", &brainAnswer->dockerIsRunning},
                {"get_docker_list", &brainAnswer->dockerList}
            };
        }
        std::string runToolCall(const nlohmann::json& call);
        void removeUselessObjects(std::vector<nlohmann::json>& base);
};

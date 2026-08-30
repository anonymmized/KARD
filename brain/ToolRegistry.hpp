#pragma once

#include "brain/OllamaBrain.hpp"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr const char* IP_TO_PING = "1.1.1.1";
constexpr int HTTPS_PORT = 443;

inline const std::string TOOL_NOT_EXIST = "There is no tool like this.";

ModelAnswer& modelAnswer;

class ToolRegistry {
    private:
        ModelAnswer& modelAnswer;
    public:
        int parsePeriod(const std::string& period);
        std::string runToolCall(const nlohmann::json& call);
        void removeUselessObjects(std::vector<nlohmann::json>& base);
};

int parsePeriod(const std::string& period);
std::string runToolCall(const nlohmann::json& call);
void removeUselessObjects(std::vector<nlohmann::json>& base);

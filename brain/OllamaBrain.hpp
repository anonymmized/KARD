#pragma once

#include "IBrain.hpp"

#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <atomic>

constexpr int MAX_TOOL_ITERATIONS = 5;
constexpr const char* IP_TO_PING = "1.1.1.1";
constexpr int HTTPS_PORT = 443;
inline const std::string TOOL_NOT_EXIST = "There is no tool like this.";

class OllamaBrain : public IBrain {
    private:
        std::string url;
        std::vector<nlohmann::json> base;
        nlohmann::json uploadConfig();
        nlohmann::json getBody();
        nlohmann::json collectAllMessages(const std::string& systemPrompt);
        void pushAllContent(const std::string& toolName);
        void pushToolContent(const std::string& content);
        int parsePeriod(const std::string& p);
    public:
        OllamaBrain() = default;
        std::string ask(const std::string& request);
};

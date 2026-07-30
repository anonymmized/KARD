#pragma once

#include "IBrain.hpp"

#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <atomic>

constexpr int MAX_TOOL_ITERATIONS = 5;
constexpr std::string TOOL_NOT_EXIST = "There is no tool like this.";

class OllamaBrain : public IBrain {
    private:
        std::string url;
        std::vector<nlohmann::json> base;
        nlohmann::json uploadConfig();
        nlohmann::json getBody();
        nlohmann::json collectAllMessages(const std::string& systemPrompt);
        void pushToolContent(const std::string& content);
        int parsePeriod(const std::string& p);
    public:
        OllamaBrain() = default;
        std::string ask(const std::string& request);
};

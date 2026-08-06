#pragma once

#include "IBrain.hpp"

#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

constexpr int MAX_TOOL_ITERATIONS = 5;

class OllamaBrain : public IBrain {
    private:
        std::string url;
        std::vector<nlohmann::json> base;
        nlohmann::json config;
        nlohmann::json uploadConfig();
        nlohmann::json getBody();
        nlohmann::json collectAllMessages(const std::string& systemPrompt);
        void pushToolContent(const std::string& content);
    public:
        OllamaBrain() {
            config = uploadConfig();
            url = config["url"];
        }
        std::string ask(const std::string& request);
};

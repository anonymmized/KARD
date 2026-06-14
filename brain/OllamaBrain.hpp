#pragma once

#include "IBrain.hpp"

#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <atomic>

class OllamaBrain : public IBrain {
    private:
        std::string url;
        std::vector<nlohmann::json> base;
        nlohmann::json json_config;
        void uploadConfig();
        nlohmann::json getBody(const std::string& request);
        int sendToLog(const std::string& tool, const std::string& value);
    public:
        OllamaBrain(std::vector<nlohmann::json> _base) : base(_base) {}
        std::string ask(const std::string& request);
};

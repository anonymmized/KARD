#pragma once

#include "contracts/IBrain.hpp"

#include <atomic>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>

constexpr int MAX_TOOL_ITERATIONS = 5;

class OllamaBrain : public IBrain {
private:
  std::string url;
  std::atomic<bool> &cancelRequesting;
  std::vector<nlohmann::json> base;
  nlohmann::json config;
  nlohmann::json uploadConfig();
  nlohmann::json getBody();
  nlohmann::json collectAllMessages(const std::string &systemPrompt);
  bool needToStop();
  void pushToolContent(const std::string &content);

public:
  OllamaBrain(std::atomic<bool> &_cancelRequesting)
      : cancelRequesting(_cancelRequesting) {
    config = uploadConfig();
    url = config["url"];
  }
  std::string ask(const std::string &request);
};

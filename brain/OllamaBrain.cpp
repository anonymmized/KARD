#include "OllamaBrain.hpp"
#include "brain/ToolRegistry.hpp"

#include <cpr/cpr.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

nlohmann::json OllamaBrain::uploadConfig() {
  std::ifstream in(CONFIG_PATH);
  nlohmann::json config;
  if (!in.is_open()) {
    std::cerr << "Unable to open config file\n";
    return config;
  }
  in >> config;
  in.close();
  return config;
}

bool OllamaBrain::needToStop() { return cancelRequesting; }

nlohmann::json OllamaBrain::collectAllMessages(const std::string &systemPrompt) {
    nlohmann::json allMessages = nlohmann::json::array();
    allMessages.push_back({{"role", "system"}, {"content", systemPrompt}});

    for (const auto &part : base) {
        allMessages.push_back(part);
    }
    return allMessages;
}

nlohmann::json OllamaBrain::getBody() {
    nlohmann::json body = config;
    body["messages"] = collectAllMessages(config["system_prompt"]);
    return body;
}

void OllamaBrain::pushToolContent(const std::string &content) {
    base.push_back({{"role", "tool"}, {"content", content}});
}

BrainAnswer OllamaBrain::ask(const std::string &request) {
    base.push_back({{"role", "user"}, {"content", request}});
    int remainingIterations = MAX_TOOL_ITERATIONS;
    std::string stringReply;
    while (remainingIterations != 0) {
        if (needToStop()) {
            brainAnswer.plainAnswer = "";
            return brainAnswer;
        }
        nlohmann::json body = getBody();

        cpr::Response resp = cpr::Post(
            cpr::Url(url), cpr::Body{body.dump()},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::ProgressCallback([this](auto, auto, auto, auto, auto) -> bool {
            return !cancelRequesting;
        }));

        if (resp.status_code != 200) {
            brainAnswer.plainAnswer = "It seems an error)\n";
            return brainAnswer;
        }

        if (needToStop()) {
            brainAnswer.plainAnswer = "";
            return brainAnswer;
        }

        auto reply = nlohmann::json::parse(resp.text);
        base.push_back(reply["message"]);
        if (!reply["message"].contains("tool_calls")) {
            stringReply = reply["message"]["content"].get<std::string>();
            brainAnswer.plainAnswer = stringReply;
            return brainAnswer;
        }

        for (const auto &call : reply["message"]["tool_calls"]) {
            std::string toolState = toolRegistry.runToolCall(call);
            pushToolContent(toolState);
        }
        toolRegistry.removeUselessObjects(base);

        stringReply = reply["message"]["content"].get<std::string>();
        remainingIterations -= 1;
    }
    brainAnswer.plainAnswer = stringReply;
    return brainAnswer;
}

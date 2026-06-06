#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <sys/ioctl.h>

int term_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_col;
    return 80;
}

nlohmann::json getBody(std::string& request, std::vector<nlohmann::json>& base) {
    nlohmann::json messages = nlohmann::json::array();
    messages.push_back({{"role", "system"},{"content", "You're a KARD. Answer short and strict."}});
    for (const auto& m : base) messages.push_back(m);
    return {
        {"model", "qwen2.5:3b"},
        {"messages", messages},
        {"options", {
            {"temperature", 0.3},
            {"top_p", 0.9},
            {"num_predict", 120}
        }},
        {"stream", false}
    };
}

nlohmann::json doRequest(nlohmann::json& body) {
    cpr::Response resp = cpr::Post(
        cpr::Url{"http://localhost:11434/api/chat"},
        cpr::Body{body.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );
    if (resp.status_code != 200) { return "It seems an error)\n"; }
    auto reply = nlohmann::json::parse(resp.text);
    //std::cout << "STATUS: " << resp.status_code << '\n';
    //std::cout << "RAW: " << resp.text << '\n';
    nlohmann::json body_reply = {{"role", reply["message"]["role"]},{"content", reply["message"]["content"]}};
    return body_reply;
}

int main() {
    std::vector<nlohmann::json> chat_base;
    std::string request;
    std::cout << "user: ";
    while (std::getline(std::cin, request)) {
        if (request == "/exit") break;
        if (request.empty()) continue;
        chat_base.push_back({{"role","user"},{"content",request}});
        nlohmann::json body = getBody(request, chat_base);
        nlohmann::json body_reply = doRequest(body);
        chat_base.push_back(body_reply);
        std::cout << body_reply["role"] << ": " << body_reply["content"] << '\n';
        std::cout << "user: ";
    }
    return 0;
}

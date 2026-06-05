#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <iomanip>
#include <unistd.h>
#include <sys/ioctl.h>

int term_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_col;
    return 80;
}

std::string doRequest(std::string& request) {
    nlohmann::json body = {
        {"model", "qwen2.5:3b"},
        {"messages", {
            {{"role","system"},{"content","You're a KARD. Answer short and strict."}},
            {{"role","user"},{"content", request}}
        }},
        {"options", {
            {"temperature", 0.3},
            {"top_p", 0.9},
            {"num_predict", 120}
        }},
        {"stream", false}
    };
    cpr::Response resp = cpr::Post(
        cpr::Url{"http://localhost:11434/api/chat"},
        cpr::Body{body.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );
    if (resp.status_code != 200) { return "It seems an error)\n"; }
    auto reply = nlohmann::json::parse(resp.text);
    //std::cout << "STATUS: " << resp.status_code << '\n';
    //std::cout << "RAW: " << resp.text << '\n';
    return reply["message"]["content"];
}

int main() {
    std::string request;
    while (std::getline(std::cin, request)) {
        if (request == "/exit") break;
        if (request.empty()) continue;
        std::cout << doRequest(request) << '\n';
    }
    return 0;
}

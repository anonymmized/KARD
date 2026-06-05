
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


int main() {
    int width = term_width();
    nlohmann::json body = {
        {"model", "llama3.2:1b"},
        {"prompt", "Why is the sky blue?"},
        {"stream", false}
    };
    cpr::Response r = cpr::Post(
        cpr::Url{"http://localhost:11434/api/generate"},
        cpr::Body{body.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );
    if (r.status_code != 200) {
        std::cout << "Bad connection\n";
        return 1;
    }
    auto reply = nlohmann::json::parse(r.text);
    std::string answer = reply["response"];
    std::cout << std::setw(width/2) << "Why is the sky blue?\n";
    std::cout << answer;
    return 0;
}

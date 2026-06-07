#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <sys/ioctl.h>
#include <fstream>

class IBrain {
    public:
        virtual ~IBrain() = default;
        virtual std::string ask(const std::string& request) = 0;

};

class OllamaBrain : public IBrain {
    private:
        std::string url = "http://localhost:11434/api/chat";
        std::vector<nlohmann::json> base;
        nlohmann::json json_config;
        void uploadConfig() {
            std::ifstream in("config.json");
            if (!in.is_open()) {
                std::cerr << "Unable to open config file\n";
                return;
            }
            in >> json_config;
            in.close();

        }
        nlohmann::json getBody(const std::string& request) {
            nlohmann::json messages = nlohmann::json::array();
            uploadConfig();
            messages.push_back({{"role", "system"},{"content", json_config["system_prompt"]}});
            for (const auto& m : base) messages.push_back(m);
            json_config.push_back({"messages",messages});
            return json_config;
        }
    public:
        OllamaBrain(std::vector<nlohmann::json>& _base) : base(_base) {}
        std::string ask(const std::string& request) {
            base.push_back({{"role", "user"},{"content", request}});
            nlohmann::json body = getBody(request);

            cpr::Response resp = cpr::Post(
                cpr::Url(url),
                cpr::Body{body.dump()},
                cpr::Header{{"Content-Type", "application/json"}}
            );

            if (resp.status_code != 200) { return "It seems an error)\n"; }

            auto reply = nlohmann::json::parse(resp.text);
            base.push_back({{"role", reply["message"]["role"]},{"content", reply["message"]["content"]}});
            return reply["message"]["content"].get<std::string>();
        }
};

int main() {
    std::vector<nlohmann::json> chat_base;
    OllamaBrain ollama(chat_base);
    IBrain& brain = ollama;
    std::string request;
    std::cout << "user: ";
    while (std::getline(std::cin, request)) {
        if (request == "/exit") break;
        if (request.empty()) continue;
        std::string answer = brain.ask(request);
        std::cout << "assistant" << ": " << answer << '\n';
        std::cout << "user: ";
    }
    return 0;
}

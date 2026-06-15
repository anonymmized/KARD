#include <chrono>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

int appendSnapshot(const std::string& tool, const std::string& value) {
    auto now = std::chrono::system_clock::now();
    std::ofstream fl(LOG_PATH, std::ios::app);
    if (!fl.is_open()) {
        std::cerr << "unable to open log file\n";
        return 1;
    }
    nlohmann::json rec = {{"ts",std::chrono::system_clock::to_time_t(now)},{"tool", tool},{"value", value}};
    fl << rec.dump() << '\n';
    return 0;
}

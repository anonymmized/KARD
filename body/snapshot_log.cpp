#include <chrono>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

int appendSnapshot(const std::string& toolName, const std::string& value) {
    auto currentTime = std::chrono::system_clock::now();
    std::ofstream fileToWrite(LOG_PATH, std::ios::app);
    if (!fileToWrite.is_open()) {
        std::cerr << "unable to open log file\n";
        return 1;
    }
    nlohmann::json snapshotReport = {{"ts",std::chrono::system_clock::to_time_t(currentTime)},{"tool", toolName},{"value", value}};
    fileToWrite << snapshotReport.dump() << '\n';
    return 0;
}

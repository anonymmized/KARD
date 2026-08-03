#include "paths.hpp"

#include <chrono>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>
#include <unordered_map>
#include <optional>

namespace {
    using SeriesMap = std::unordered_map<std::string, std::vector<std::pair<long, double>>>;
    const std::set<std::string> NUMERIC_TOOLS = {"get_cpu", "get_ram", "get_temp"};
    struct InVars {
        time_t nowTimeStamp;
        int windowSeconds;
    };

    struct OutVars {
        std::string toolName;
        long timeStamp;
        double value;
    };

    std::optional<OutVars> processLine(const nlohmann::json& parsedLine, const InVars& inVars) {
        long timeStamp = parsedLine["ts"].get<long>();
        if (inVars.nowTimeStamp - timeStamp > inVars.windowSeconds) {
            return std::nullopt;
        }
        std::string toolName = parsedLine["tool"].get<std::string>();
        if (!NUMERIC_TOOLS.count(toolName)) {
            return std::nullopt;
        }
        double value = std::stod(parsedLine["value"].get<std::string>());
        OutVars outVars = {toolName, timeStamp, value};
        return outVars;
    }
}

std::ifstream openLogFile() {
    return std::ifstream(getPathToLogFile());
}

bool compareByTimeStamp(const std::pair<long, double>& firstPair, const std::pair<long, double>& secondPair) {
    return firstPair.first < secondPair.first;
}

std::string buildSummary(SeriesMap& series) {
    std::string summary;
    for (auto& [toolName, timeStampsVec] : series) {
        std::sort(timeStampsVec.begin(), timeStampsVec.end(), compareByTimeStamp);
        double sum = 0.0;
        double maxTimeStamp = timeStampsVec.front().second;
        for (const auto& [timeStamp, value] : timeStampsVec) {
            sum += value;
            maxTimeStamp = std::max(maxTimeStamp, value);
        }
        double valueAvg = sum / timeStampsVec.size();
        double valueGrowth = timeStampsVec.back().second - timeStampsVec.front().second;
        summary += toolName + ": avg " + std::to_string(valueAvg) + ", peak " + std::to_string(maxTimeStamp) + ", change " + std::to_string(valueGrowth) + " (" + std::to_string(timeStampsVec.size()) + " samples)\n";
    }
    return summary;
}

std::string summarizeHealth(const int seconds) {
    std::ifstream logFile = openLogFile();
    if (!logFile.is_open()) {
        return "unable to open log file\n";
    }
    auto now = std::chrono::system_clock::now();
    auto nowTimePoint = std::chrono::system_clock::to_time_t(now);
    InVars inVars = {nowTimePoint, seconds};
    SeriesMap series;

    std::string line;
    while (std::getline(logFile, line)) {
        if (line.empty()) {
            continue;
        }
        nlohmann::json parsedLine = nlohmann::json::parse(line, nullptr, false);
        if (parsedLine.is_discarded()) {
            continue;
        }
        std::optional<OutVars> outVars = processLine(parsedLine, inVars);
        if (!outVars) {
            continue;
        }
        series[outVars->toolName].push_back({outVars->timeStamp, outVars->value});
    }

    std::string finalString = buildSummary(series);
    return finalString.empty() ? "no data for the last " + std::to_string(seconds / 3600) + "h." : finalString;
}

int appendSnapshot(const std::string& toolName, const std::string& value) {
    auto currentTime = std::chrono::system_clock::now();
    std::string logFilePath = getPathToLogFile();
    std::ofstream fileToWrite(logFilePath, std::ios::app);
    if (!fileToWrite.is_open()) {
        std::cerr << "unable to open log file\n";
        return 1;
    }
    nlohmann::json snapshotReport = {{"ts",std::chrono::system_clock::to_time_t(currentTime)},{"tool", toolName},{"value", value}};
    fileToWrite << snapshotReport.dump() << '\n';
    return 0;
}

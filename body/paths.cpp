#include "paths.hpp"
#include <filesystem>
#include <string>
#include <cstdlib>

std::string pathToLogFile() {
    const char* homeDir = std::getenv("HOME");
    std::string kardDirectory = std::string(homeDir ? homeDir : ".") + "/.kard";
    createDirectory(kardDirectory);
    return kardDirectory + "/model_sys_log.jsonl";
}

bool createDirectory(const std::string& directoryToCreate) {
    return std::filesystem::create_directories(directoryToCreate);
}

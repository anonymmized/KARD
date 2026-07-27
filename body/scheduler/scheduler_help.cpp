#include "body/scheduler/scheduler_help.hpp"

#include <cstdlib>
#include <fstream>

namespace scheduler::detail {
    bool run(const std::string& command) {
        return std::system(command.c_str()) == 0;
    }

    bool writeFile(const std::string& path, const std::string& textToWrite) {
        std::ofstream outFile(path, std::ios::trunc);
        if (!outFile.is_open()) {
            return false;
        }
        outFile << textToWrite;
        return true;
    }

    std::optional<std::string> resolveHomeDir() {
        const char* homePath = std::getenv("HOME");
        if (!homePath) {
            return std::nullopt;
        }
        return std::string(homePath);
    }
}

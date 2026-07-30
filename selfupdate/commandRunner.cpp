#include "selfupdate/commandRunner.hpp"

#include <filesystem>
#include <string>
#include <vector>

void CommandRunner::executeCommand(const std::string& command) {
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("command execution failed");
    }
}

void CommandRunner::executeCommands(const std::vector<std::string>& commands) {
    for (const auto& command : commands) {
        executeCommand(command);
    }
}

#include "selfupdate/commandBuilder.hpp"

#include <string>
#include <vector>

std::string CommandBuilder::getCommandToClone() {
    return "git clone --depth 1 " + REPO_URL + " " + projectPath;
}

std::string CommandBuilder::getCommandToConfigureMake() {
    return "cmake -S " + projectPath + " -B " + projectPath + "/build";
}

std::string CommandBuilder::getCommandToMake() {
    return "cmake --build " + projectPath + "/build > " + logPath + " 2>&1";
}

std::vector<std::string> CommandBuilder::makeCommands() {
    std::vector<std::string> commands;

    commands.push_back(getCommandToClone());
    commands.push_back(getCommandToConfigureMake());
    commands.push_back(getCommandToMake());
    return commands;
}

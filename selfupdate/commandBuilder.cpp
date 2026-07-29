#include "selfupdate/commandBuilder.hpp"

#include <string>
#include <vector>
#include <optional>

std::string CommandBuilder::getCommandToClone() {
    return "git clone --depth 1 " + REPO_URL + " " + pathToProject;
}

std::string CommandBuilder::getCommandToConfigureMake() {
    return "cmake -S " + pathToProject + " -B " + pathToProject + "/build";
}

std::string CommandBuilder::getCommandToMake() {
    return "cmake --build " + pathToProject + "/build > " + logPathToUpdate + " 2>&1";
}

std::vector<std::string> CommandBuilder::makeCommands(const std::string& _logPathToUpdate) {
    logPathToUpdate = _logPathToUpdate;
    std::vector<std::string> commands;

    commands.push_back(getCommandToClone());
    commands.push_back(getCommandToConfigureMake());
    commands.push_back(getCommandToMake());
    return commands;
}

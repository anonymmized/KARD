#pragma once

#include <string>
#include <optional>
#include <vector>

class CommandBuilder {
    private:
        const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
        std::string pathToProject;
        std::string logPathToUpdate;

        std::string getCommandToClone();
        std::string getCommandToConfigureMake();
        std::string getCommandToMake();
    public:
        CommandBuilder(const std::string& tempPath, const std::optional<std::string>& _pathToProject) {
            pathToProject = _pathToProject.value_or(tempPath)
        }
        std::vector<std::string> makeCommands(const std::string& _logPathToUpdate);
};

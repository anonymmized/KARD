#pragma once

#include <string>
#include <vector>

class CommandBuilder {
    private:
        const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
        std::string projectPath;
        std::string logPath;

        std::string getCommandToClone();
        std::string getCommandToConfigureMake();
        std::string getCommandToMake();
    public:
        CommandBuilder(const std::string& _projectPath, const std::string& _logPath) : projectPath(_projectPath), logPath(_logPath) {}
        std::vector<std::string> makeCommands();
};

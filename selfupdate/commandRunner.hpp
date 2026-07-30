#pragma once

#include <string>
#include <vector>

class CommandRunner {
    private:
        void executeCommand(const std::string& command);
    public:
        void executeCommands(const std::vector<std::string>& commands);
};

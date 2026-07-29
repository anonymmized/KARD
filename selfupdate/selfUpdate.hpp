#pragma once

#include <filesystem>
#include <string>
#include <optional>
#include <vector>

struct UpdaterHelper {
    std::string HOME_PATH;
    std::string LOG_PATH_TO_UPDATE;
    std::string TEMP_PATH;
    std::string PATH_TO_SELF;
};

class Updater {
    private:
        constexpr std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
        UpdaterHelper allPaths;

        void loadAllPaths();

        std::string getCommandToClone(const std::string& pathToClone);
        std::string getCommandToConfigureMake(const std::string& pathToProject);
        std::string getCommandToMake(const std::string& pathToProject);

        void removeDirectory(const std::string& directoryToRemove);
        void executeCommand(const std::string& command);
        void executeCommands(const std::vector<std::string>& commands);
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameTargetFile(const std::string& newName, const std::string& oldName);

        std::vector<std::string> makeCommands(const std::optional<std::string>& targetPath);
    public:
        Updater() {
            loadAllPaths();
        }

        ~Updater() {
            removeDirectory(allPaths.TEMP_PATH);
        }

        int runBinaryFileUpdate();
        int runFullUpdate();
};


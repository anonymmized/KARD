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
        UpdaterHelper allPaths;

        void loadAllPaths();

        void removeDirectory(const std::string& directoryToRemove);
        void executeCommand(const std::string& command);
        void executeCommands(const std::vector<std::string>& commands);
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameTargetFile(const std::string& newName, const std::string& oldName);
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


#pragma once

#include "selfupdate/updatePaths.hpp"

#include <filesystem>
#include <string>
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
        PathsUpdater pathsUpdater;
        void loadAllPaths();

        void removeDirectory(const std::string& directoryToRemove);
        void executeCommand(const std::string& command);
        void executeCommands(const std::vector<std::string>& commands);
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameTargetFile(const std::string& oldName, const std::string& newName);
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


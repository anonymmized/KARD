#pragma once

#include <filesystem>
#include <string>

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

        void setTempDirectory();
        void setPathToSelf();
        void setHomePath();
        void setLogPath();

        std::string getCommandToClone(const std::string& pathToClone);
        std::string getCommandToConfigureMake(const std::string& pathToProject);
        std::string getCommandToMake(const std::string& pathToProject);
    public:
        Updater() {
            setTempDirectory();
            setPathToSelf();
            setHomePath();
            setLogPath();
        }

        ~Updater() {
            removeDirectory(allPaths.TEMP_PATH);
        }

        void removeDirectory(const std::string& directoryToRemove);

        void executeCommand(const std::string& command);
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameTargetFile(const std::string& newName, const std::string& oldName);

        int runBinaryFileUpdate();
        int runFullUpdate();
};


#pragma once

#include <filesystem>
#include <string>

class Updater {
    private:
        const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
        std::string HOME_PATH;
        std::string UPDATE_LOG_PATH;
        std::filesystem::path TEMP_DIRECTORY;
        std::string PATH_TO_SELF;

        void setTempDirectory();
        void setPathToSelf();
        void setHomePath();
        void setLogPath();

        std::string getCommandToClone();
        std::string getCommandToConfigureMake();
        std::string getCommandToMake();
    public:
        Updater() {
            setTempDirectory();
            setPathToSelf();
            setHomePath();
            setLogPath();
        }

        ~Updater() {
            removeDirectory(TEMP_DIRECTORY.string());
        }

        void removeDirectory(const std::string& directoryToRemove);

        void executeCommand(const std::string& command);
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameTargetFile(const std::string& newName, const std::string& oldName);

        int runBinaryFileUpdate();
};


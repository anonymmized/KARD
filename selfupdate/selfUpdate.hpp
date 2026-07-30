#pragma once

#include "selfupdate/updatePaths.hpp"
#include "selfupdate/commandRunner.hpp"
#include "selfupdate/fileOps.hpp"

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
        CommandRunner commandRunner;
        FileOps fileOps;
        void loadAllPaths();

    public:
        Updater() {
            loadAllPaths();
        }

        ~Updater() {
            fileOps.removeDirectory(allPaths.TEMP_PATH);
        }

        int runBinaryFileUpdate();
        int runFullUpdate();
};


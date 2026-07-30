#include "selfupdate/selfUpdate.hpp"
#include "selfupdate/updatePaths.hpp"
#include "selfupdate/commandBuilder.hpp"
#include "selfupdate/commandRunner.hpp"
#include "selfupdate/fileOps.hpp"

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#endif

void Updater::loadAllPaths() {
    PathsUpdater pathsUpdater;
    allPaths.HOME_PATH = pathsUpdater.getHomePath();
    allPaths.LOG_PATH_TO_UPDATE = pathsUpdater.getLogPathToUpdate();
    allPaths.TEMP_PATH = pathsUpdater.getTempPath();
    allPaths.PATH_TO_SELF = pathsUpdater.getPathToSelf();
}

int Updater::runBinaryFileUpdate() {
    std::string pathToBinaryFile = allPaths.TEMP_PATH + "/build/kard";
    std::string pathToNewBinary = allPaths.PATH_TO_SELF + ".new";

    CommandBuilder commandBuilder(allPaths.TEMP_PATH, allPaths.LOG_PATH_TO_UPDATE);
    std::vector<std::string> commands = commandBuilder.makeCommands();

    try {
        commandRunner.executeCommands(commands);
        fileOps.copyFile(pathToBinaryFile, pathToNewBinary);
        fileOps.renameFile(pathToNewBinary, allPaths.PATH_TO_SELF);
    } catch (const std::exception& e) {
        std::cerr << "update failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}

int Updater::runFullUpdate() {
    std::string pathToProject = std::filesystem::path(allPaths.PATH_TO_SELF).parent_path().parent_path().string();
    std::string pathToNewProject = pathToProject + ".new";

    CommandBuilder commandBuilder(pathToNewProject, allPaths.LOG_PATH_TO_UPDATE);
    std::vector<std::string> commands = commandBuilder.makeCommands();

    try {
        commandRunner.executeCommands(commands);

        fileOps.renameFile(pathToProject, pathToProject + ".old");
        fileOps.renameFile(pathToNewProject, pathToProject);
        fileOps.removeDirectory(pathToProject + ".old");
    } catch (const std::exception& exception) {
        std::cerr << "update failed: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}

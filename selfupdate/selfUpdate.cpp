#include "selfupdate/selfUpdate.hpp"
#include "selfupdate/updatePaths.hpp"
#include "selfupdate/commandBuilder.hpp"

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <vector>
#include <optional>

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

void Updater::removeDirectory(const std::string& directoryToRemove) {
    std::error_code errorCode;
    std::filesystem::remove_all(directoryToRemove, errorCode);
    if (errorCode) {
        std::cerr << "remove failed: " << errorCode.message() << '\n';
    }
}

void Updater::executeCommand(const std::string& command) {
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("command execution failed");
    }
}

void Updater::copyFile(const std::string& oldFile, const std::string& newFile) {
    std::error_code errorCode;

    std::filesystem::copy_file(oldFile, newFile, std::filesystem::copy_options::overwrite_existing, errorCode);

    if (errorCode) {
        throw std::runtime_error(std::string("copy failed: " + errorCode.message()));
    }
}

void Updater::renameTargetFile(const std::string& oldName, const std::string& newName) {
    std::error_code errorCode;

    std::filesystem::rename(oldName, newName, errorCode);

    if (errorCode) {
        throw std::runtime_error(std::string("rename failed: " + errorCode.message()));
    }
}

void Updater::executeCommands(const std::vector<std::string>& commands) {
    for (const auto& command : commands) {
        executeCommand(command);
    }
}

int Updater::runBinaryFileUpdate() {
    std::string pathToBinaryFile = allPaths.TEMP_PATH + "/build/kard";
    std::string pathToNewBinary = allPaths.PATH_TO_SELF + ".new";

    CommandBuilder commandBuilder(allPaths.TEMP_PATH, allPaths.LOG_PATH_TO_UPDATE);
    std::vector<std::string> commands = commandBuilder.makeCommands();

    try {
        executeCommands(commands);
        copyFile(pathToBinaryFile, pathToNewBinary);
        renameTargetFile(pathToNewBinary, allPaths.PATH_TO_SELF);
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
        executeCommands(commands);

        renameTargetFile(pathToProject, pathToProject + ".old");
        renameTargetFile(pathToNewProject, pathToProject);
        removeDirectory(pathToProject + ".old");
    } catch (const std::exception& exception) {
        std::cerr << "update failed: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}

#include "selfUpdate.hpp"

#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <string>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#endif

void Updater::setTempDirectory() {
    try {
        allPaths.TEMP_PATH = (std::filesystem::temp_directory_path() / ("kard-update" + std::to_string(getpid()))).string();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("temp directory set error: ") + e.what());
    }
}

void Updater::setPathToSelf() {
#ifdef __APPLE__
    char bufferForPath[PATH_MAX];
    uint32_t bufferSize = sizeof(bufferForPath);
    if (_NSGetExecutablePath(bufferForPath, &bufferSize) != 0) {
        allPaths.PATH_TO_SELF = "";
        return;
    }
    allPaths.PATH_TO_SELF = std::filesystem::canonical(bufferForPath).string();
#else
    allPaths.PATH_TO_SELF = std::filesystem::canonical("/proc/self/exe").string();
#endif
}

void Updater::setHomePath() {
    const char* homePath = std::getenv("HOME");
    allPaths.HOME_PATH = homePath ? homePath : ".";
}

void Updater::setLogPath() {
    allPaths.LOG_PATH_TO_UPDATE = allPaths.HOME_PATH + "/.kard/update.log";
}

std::string Updater::getCommandToClone(const std::string& pathToClone) {
    return "git clone --depth 1 " + REPO_URL + " " + pathToClone;
}

std::string Updater::getCommandToConfigureMake(const std::string& pathToProject) {
    return "cmake -S " + pathToProject + " -B " + pathToProject + "/build";
}

std::string Updater::getCommandToMake(const std::string& pathToProject) {
    return "cmake --build " + pathToProject + "/build > " + allPaths.LOG_PATH_TO_UPDATE + " 2>&1";
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

int Updater::runBinaryFileUpdate() {
    std::string pathToBinaryFile = allPaths.TEMP_PATH + "/build/kard";
    std::string pathToNewBinary = allPaths.PATH_TO_SELF + ".new";

    std::string commandToClone = getCommandToClone(allPaths.TEMP_PATH);
    std::string commandToConfigureMake = getCommandToConfigureMake(allPaths.TEMP_PATH);
    std::string commandToMake = getCommandToMake(allPaths.TEMP_PATH);
    try {
        executeCommand(commandToClone);
        executeCommand(commandToConfigureMake);
        executeCommand(commandToMake);
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

    std::string commandToClone = getCommandToClone(pathToNewProject);
    std::string commandToConfigureMake = getCommandToConfigureMake(pathToNewProject);
    std::string commandToMake = getCommandToMake(pathToNewProject);

    try {
        executeCommand(commandToClone);
        executeCommand(commandToConfigureMake);
        executeCommand(commandToMake);

        renameTargetFile(pathToProject, pathToProject + ".old");
        renameTargetFile(pathToNewProject, pathToProject);
        removeDirectory(pathToProject + ".old");
    } catch (const std::exception& exception) {
        std::cerr << "update failed: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}

#include "selfUpdate.hpp"

#include <iostream>
#include <filesystem>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#endif

void Updater::setTempDirectory() {
    try {
        TEMP_DIRECTORY = std::filesystem::temp_directory_path() / ("kard-update" + std::to_string(getpid()));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("temp directory set error: ") + e.what());
    }
}

void Updater::setPathToSelf() {
#ifdef __APPLE__
    char bufferForPath[PATH_MAX];
    uint32_t bufferSize = sizeof(bufferForPath);
    if (_NSGetExecutablePath(bufferForPath, &bufferSize) != 0) {
        PATH_TO_SELF = "";
        return;
    }
    PATH_TO_SELF = std::filesystem::canonical(bufferForPath).string();
#else
    PATH_TO_SELF = std::filesystem::canonical("/proc/self/exe").string();
#endif
}

void Updater::setHomePath() {
    const char* homePath = std::getenv("HOME");
    HOME_PATH = homePath ? homePath : ".";
}

void Updater::setLogPath() {
    UPDATE_LOG_PATH = HOME_PATH + "/.kard/update.log";
}

std::string Updater::getCommandToClone() {
    return "git clone --depth 1 " + REPO_URL + " " + TEMP_DIRECTORY.string();
}

std::string Updater::getCommandToConfigureMake() {
    return "cmake -S " + TEMP_DIRECTORY.string() + " -B " + TEMP_DIRECTORY.string() + "/build";
}

std::string Updater::getCommandToMake() {
    return "cmake --build " + TEMP_DIRECTORY.string() + "/build > " + UPDATE_LOG_PATH + " 2>&1";
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

void Updater::renameTargetFile(const std::string& newName, const std::string& oldName) {
    std::error_code errorCode;

    std::filesystem::rename(newName, oldName, errorCode);

    if (errorCode) {
        throw std::runtime_error(std::string("rename failed: " + errorCode.message()));
    }
}

int Updater::runBinaryFileUpdate() {
    std::string pathToBinaryFile = TEMP_DIRECTORY.string() + "/build/kard";
    std::string pathToNewBinary = PATH_TO_SELF + ".new";

    std::string commandToClone = getCommandToClone();
    std::string commandToConfigureMake = getCommandToConfigureMake();
    std::string commandToMake = getCommandToMake();
    try {
        executeCommand(commandToClone);
        executeCommand(commandToConfigureMake);
        executeCommand(commandToMake);
        copyFile(pathToBinaryFile, pathToNewBinary);
        renameTargetFile(pathToNewBinary, PATH_TO_SELF);
    } catch (const std::exception& e) {
        std::cerr << "update failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}

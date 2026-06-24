#include <iostream>
#include <filesystem>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#endif

std::string getPathToSelf() {
#ifdef __APPLE__
    char bufferForPath[PATH_MAX];
    uint32_t buffersSize = sizeof(bufferForPath);
    if (_NSGetExecutablePath(bufferForPath, &buffersSize) != 0) {
        return "";
    }
    return std::filesystem::canonical(bufferForPath).string();
#else
    return std::filesystem::canonical("/proc/self/exe").string();
#endif
}

std::string getHomePath() {
    const char* homePath = std::getenv("HOME");
    return homePath ? homePath : ".";
}

const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
const std::string HOME_PATH = getHomePath();
const std::string UPDATE_LOG_PATH = HOME_PATH + "/.kard/update.log";

std::filesystem::path createTempDirectory() {
    return std::filesystem::temp_directory_path() / ("kard-update-" + std::to_string(getpid()));
}

std::string getCommandToClone(const std::filesystem::path& tempDirectory) {
    return "git clone --depth 1 " + REPO_URL +  " " + tempDirectory.string();
}

std::string getCommandToConfigureMake(const std::filesystem::path& tempDirectory) {
    return "cmake -S " + tempDirectory.string() + " -B " + tempDirectory.string() + "/build";
}

std::string getCommandToMake(const std::filesystem::path& tempDirectory) {
    return "cmake --build " + tempDirectory.string() + "/build > " + UPDATE_LOG_PATH + " 2>&1";
}

int executeCommand(const std::string& command) {
    if (std::system(command.c_str()) != 0) {
        std::cerr << "command execution failed\n";
        return 1;
    }
    return 0;
}

int copyFile(const std::string& oldFile, const std::string& newFile) {
    std::error_code errorCode;

    std::filesystem::copy_file(oldFile, newFile, std::filesystem::copy_options::overwrite_existing, errorCode);

    if (errorCode) {
        std::cerr << "copy failed: " << errorCode.message() << '\n';
        return 1;
    }
    return 0;
}

int renameTargetFile(const std::string& newName, const std::string& oldName) {
    std::error_code errorCode;

    std::filesystem::rename(newName, oldName, errorCode);

    if (errorCode) {
        std::cerr << "rename failed: " << errorCode.message() << '\n';
        return 1;
    }
    return 0;
}

void removeDirectory(const std::string& directoryToRemove) {
    std::error_code errorCode;
    std::filesystem::remove_all(directoryToRemove, errorCode);
    if (errorCode) {
        std::cerr << "remove failed: " << errorCode.message() << '\n';
    }
}

int runBinaryFileUpdate() {
    std::filesystem::path tempDirectory = createTempDirectory();
    std::string pathToBinaryFile = tempDirectory.string() + "/build/kard";
    std::string pathToCurrentBinary = getPathToSelf();
    std::string pathToNewBinary = pathToCurrentBinary + ".new";

    std::string commandToClone = getCommandToClone(tempDirectory);
    if (executeCommand(commandToClone) != 0) {
        return 1;
    }

    std::string commandToConfigureMake = getCommandToConfigureMake(tempDirectory);
    if (executeCommand(commandToConfigureMake) != 0) {
        removeDirectory(tempDirectory);
        return 1;
    }

    std::string commandToMake = getCommandToMake(tempDirectory);
    if (executeCommand(commandToMake) != 0) {
        removeDirectory(tempDirectory);
        return 1;
    }

    if (copyFile(pathToBinaryFile, pathToNewBinary) != 0) {
        removeDirectory(tempDirectory);
        return 1;
    }

    if (renameTargetFile(pathToNewBinary, pathToCurrentBinary) != 0) {
        removeDirectory(tempDirectory);
        return 1;
    }

    removeDirectory(tempDirectory);

    return 0;
}


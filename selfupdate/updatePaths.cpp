#include "selfupdate/updatePaths.hpp"

#include <string>
#include <filesystem>
#include <unistd.h>
#include <stdexcept>
#include <cstdint>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <climits>
#endif

void PathsUpdater::setHomePath() {
    const char* homePath = std::getenv("HOME");
    HOME_PATH = homePath ? homePath : ".";
}

void PathsUpdater::setLogPathToUpdate() {
    LOG_PATH_TO_UPDATE = HOME_PATH + "/.kard/update.log";
}

void PathsUpdater::setTempPath() {
    try {
        TEMP_PATH = (std::filesystem::temp_directory_path() / ("kard-update" + std::to_string(getpid()))).string();
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("temp directory set error: ") + e.what());
    }
}

void PathsUpdater::setPathToSelf() {
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

void PathsUpdater::setAllPaths() {
    setHomePath();
    setLogPathToUpdate();
    setTempPath();
    setPathToSelf();
}

std::string PathsUpdater::getHomePath() { return HOME_PATH; }
std::string PathsUpdater::getLogPathToUpdate() { return LOG_PATH_TO_UPDATE; }
std::string PathsUpdater::getTempPath() { return TEMP_PATH; }
std::string PathsUpdater::getPathToSelf() { return PATH_TO_SELF; }

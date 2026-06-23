#include <iostream>
#include <filesystem>
#include <unistd.h>

const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";
const std::string homePath = std::getenv("HOME");
const std::string UPDATE_LOG_PATH = homePath + "/.kard/update.log";

std::filesystem::path createTempDirectory() {
    return std::filesystem::temp_directory_path() / ("kard-update-" + std::to_string(getpid()));
}

std::string getCommandToClone(const std::filesystem::path tempDirectory) {
    return "git clone --depth 1 " + REPO_URL +  " " + tempDirectory.string();
}

int main() {
    std::filesystem::path tempDirectory = createTempDirectory();
    std::string commandToClone = getCommandToClone(tempDirectory);
    if (std::system(commandToClone.c_str()) != 0) {
        std::cout << "clone failed\n";
        return 1;
    }
    std::cout << "Repo cloned to: " << tempDirectory << '\n';
    std::string commandToConfigureMake = "cmake -S " + tempDirectory.string() + " -B " + tempDirectory.string() + "/build";

    if (std::system(commandToConfigureMake.c_str()) != 0) {
        std::cout << "Can't configure make\n";
        return 1;
    }

    std::string commandToMake = "cmake --build " + tempDirectory.string() + "/build > " + UPDATE_LOG_PATH + " 2>&1";
    if (std::system(commandToMake.c_str()) != 0) {
        std::cout << "there is a make problem. to see what happens whatch update.log\n";
        return 1;
    }
    std::cout << "Project built in temporary directory\n";

    std::filesystem::remove_all(tempDirectory);
    std::cout << "Temporary directory removed\n";
    return 0;
}



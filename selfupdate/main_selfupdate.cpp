#include <iostream>
#include <filesystem>
#include <unistd.h>

const std::string REPO_URL = "https://github.com/anonymmized/KARD.git";

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
    std::cout << "Repo cloned to: " << tempDirectory;
    return 0;
}



#include <filesystem>
#include <cstdio>
#include <iostream>
#include <string>

#include "packageDownload.hpp"

/*
std::string getCommandInString(const std::string& command) {
    std::string result;
    char buffer[256];

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return result;
    }

    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}
*/


int main() {
    int exitCode = installMissingPackages();
    switch (exitCode) {
        case -1:
            std::cout << "There is no package manager to use\n";
            break;
        case 1:
            std::cout << "Can't validate sudo permissions\n";
            break;
        case 2:
            break;
        case 0:
            std::cout << "All packages installed✌️\n";
            break;
        default:
            break;
    }
    return 0;
}

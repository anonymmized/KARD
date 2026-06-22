#include "packageDownload.hpp"

#include <vector>
#include <string>
#include <filesystem>
#include <cstdio>
#include <iostream>

int installMissingPackages() {
    std::vector<std::string> packagesToInstall = getMissingPackages();
    if (packagesToInstall.empty()) {
        return 0;
    }

    std::string packetManager = getPacketManager();
    if (packetManager == "none") {
        return -1;
    }

    if (!validateSudo()) {
        return 1;
    }

    std::string installCommand = getCommandToInstall(packetManager);
    for (auto& package : packagesToInstall) {
        if (std::system((installCommand + package).c_str()) != 0) {
            std::cout << "Can't install package: " << package << '\n';
            return 2;
        }
    }
    return 0;
}

std::string getPacketManager() {
    for (auto* candidate : CANDIDATES) {
        if (isInstalled(candidate)) {
            return candidate;
        }
    }
    return "none";
}

bool validateSudo() {
    if (std::system("sudo -v") != 0) {
        return false;
    }
    return true;
}

std::string getCommandToInstall(const std::string& packetManager) {
    if (packetManager == "apt-get") {
        return "sudo apt-get install -y ";
    }
    if (packetManager == "dnf") {
        return "sudo dnf install -y ";
    }
    if (packetManager == "yum") {
        return "sudo yum install -y ";
    }
    if (packetManager == "zypper") {
        return "sudo zypper --non-interactive install ";
    }
    if (packetManager == "pacman") {
        return "sudo pacman -S --noconfirm ";
    }
    if (packetManager == "apk") {
        return "sudo apk add ";
    }
    if (packetManager == "brew") {
        return "brew install ";
    }
    if (packetManager == "port") {
        return "sudo port install ";
    }
    return "";
}

std::vector<std::string> getMissingPackages() {
    std::vector<std::string> needToInstall;
    for (auto* tool : {"git", "cmake", "g++", "make"}) {
        if (!isInstalled(tool)) {
            needToInstall.push_back(tool);
        }
    }
    return needToInstall;
}

bool isInstalled(const std::string& tool) {
    int installed = std::system(("command -v " + tool + " >/dev/null 2>&1").c_str());
    if (installed) return 0;
    return 1;
}

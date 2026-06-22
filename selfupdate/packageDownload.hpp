#pragma once

#include <vector>
#include <string>

constexpr const char* CANDIDATES[] = {
#ifdef __APPLE__
    "brew", "port",
#else
    "apt-get", "dnf", "yum", "pacman", "zypper", "apk", "brew"
#endif
};

int installMissingPackages();
std::string getPacketManager();
bool validateSudo();
std::string getCommandToInstall(const std::string& packetManager);
std::vector<std::string> getMissingPackages();
bool isInstalled(const std::string& tool);

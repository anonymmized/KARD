#include "dockerMonitoring.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <cpr/cpr.h>

DockerChecker::DockerChecker() :
    HOME_DIRECTORY(std::getenv("HOME") ? std::getenv("HOME") : ""),
    DOCKER_HOST( std::getenv("DOCKER_HOST") ? std::getenv("DOCKER_HOST") : "") {}

bool DockerChecker::isDockerRunning() {
    if (!DOCKER_HOST.empty()) {
        addToCandidates(DOCKER_HOST);
    }
    if (!HOME_DIRECTORY.empty()) {
        std::string candidateFromHome = HOME_DIRECTORY + MAC_PATH_TO_SOCKET;
        addToCandidates(candidateFromHome);
    }
    addToCandidates(LINUX_PATH_TO_SOCKET);

    if (findDockerSocket()) {
        return true;
    }

    return false;
}

void DockerChecker::addToCandidates(const std::string& candidateToAdd) {
    if (isPrefixInPath(candidateToAdd)) {
        candidates.push_back(candidateToAdd.substr(PREFIX.size()));
    }
    candidates.push_back(candidateToAdd);
}

bool DockerChecker::isPrefixInPath(const std::string& candidate) {
    return candidate.rfind(PREFIX, 0) == 0;
}

bool DockerChecker::findDockerSocket() {
    for (const std::string& candidate : candidates) {
        if (pingDocker(candidate)) {
            return true;
        }
    }
    return false;
}

bool DockerChecker::pingDocker(const std::string& candidatePath) {
    cpr::Response response = cpr::Get(cpr::Url{DOCKER_URL}, cpr::UnixSocket{candidatePath}, cpr::Timeout{1000});

    return response.status_code == 200;
}

int main () {
    DockerChecker checker;
    if (checker.isDockerRunning()) {
        std::cout << "docker is running\n";
        return 0;
    }
    std::cout << "docker is not running\n";
    return 1;
}

#include "dockerMetrics.hpp"

#include <string>
#include <vector>
#include <cstdlib>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

DockerChecker::DockerChecker() :
    HOME_DIRECTORY(std::getenv("HOME") ? std::getenv("HOME") : ""),
    DOCKER_HOST( std::getenv("DOCKER_HOST") ? std::getenv("DOCKER_HOST") : "") {
        addCandidateByPath(DOCKER_HOST);
        addCandidateByPath(HOME_DIRECTORY);
        addToCandidates(LINUX_PATH_TO_SOCKET);
}

bool DockerChecker::isDockerRunning() {
    if (findDockerSocket()) {
        return true;
    }

    return false;
}

void DockerChecker::addCandidateByPath(const std::string& pathToCheck) {
    if (!pathToCheck.empty()) {
        if (pathToCheck == HOME_DIRECTORY) {
            std::string candidateFromHome = HOME_DIRECTORY + MAC_PATH_TO_SOCKET;
            addToCandidates(candidateFromHome);
            return;
        }
        addToCandidates(pathToCheck);
    }
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
    cpr::Response response = cpr::Get(cpr::Url{DOCKER_PING_URL}, cpr::UnixSocket{candidatePath}, cpr::Timeout{1000});

    return response.status_code == 200;
}

int DockerChecker::getCountRunningContainers() {
    for (const std::string& candidate : candidates) {
        int runningCount = findCountRunningContainers(candidate);
        if (runningCount != -1) {
            return runningCount;
        }
    }
    return -1;
}

int DockerChecker::findCountRunningContainers(const std::string& candidatePath) {
    cpr::Response response = cpr::Get(cpr::Url{DOCKER_NUM_CONT_URL}, cpr::UnixSocket{candidatePath}, cpr::Timeout{1000});

    if (response.status_code != 200) {
        return -1;
    }

    auto containers = nlohmann::json::parse(response.text, nullptr, false);
    if (containers.is_discarded() || !containers.is_array()) {
        return -1;
    }
    return static_cast<int>(containers.size());
}

std::string checkIfDockerIsRunning() {
    DockerChecker checker;
    std::string dockerStatusText = std::string("docker is running: ") + (checker.isDockerRunning() ? "true" : "false");
    return dockerStatusText;
}

std::string getNumOfRunningContainers() {
    DockerChecker checker;
    std::string dockerCountText = "docker running containers count: " + std::to_string(checker.getCountRunningContainers());
    return dockerCountText;
}

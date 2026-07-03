#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class DockerChecker {
    private:
        const std::string PREFIX = "unix://";
        const std::string MAC_PATH_TO_SOCKET = "/.docker/run/docker.sock";
        const std::string LINUX_PATH_TO_SOCKET = "/var/run/docker.sock";
        const std::string DOCKER_PING_URL = "http://localhost/_ping";
        const std::string DOCKER_NUM_CONT_URL = "http://localhost/containers/json";
        const std::string HOME_DIRECTORY;
        const std::string DOCKER_HOST;

        std::vector<std::string> candidates;

        std::vector<nlohmann::json> containersResponses;

        void setContainersResponses();

        void addToCandidates(const std::string& candidateToAdd);
        bool pingDocker(const std::string& candidatePath);
        bool isPrefixInPath(const std::string& candidate);
        bool findDockerSocket();
        void addCandidateByPath(const std::string& pathToCheck);

    public:
        DockerChecker();
        bool isDockerRunning();
        int getCountRunningContainers();
        std::string listContainers();
};

std::string getContainersList();
std::string checkIfDockerIsRunning();
std::string getNumOfRunningContainers();

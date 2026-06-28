#pragma once

#include <string>
#include <vector>

class DockerChecker {
    private:
        const std::string PREFIX = "unix://";
        const std::string MAC_PATH_TO_SOCKET = "/.docker/run/docker.sock";
        const std::string LINUX_PATH_TO_SOCKET = "/var/run/docker.sock";
        const std::string DOCKER_URL = "http://localhost/_ping";
        const std::string HOME_DIRECTORY;
        const std::string DOCKER_HOST;

        std::vector<std::string> candidates;

        void addToCandidates(const std::string& candidateToAdd);
        bool pingDocker(const std::string& candidatePath);
        bool isPrefixInPath(const std::string& candidate);
        bool findDockerSocket();

    public:
        DockerChecker();
        bool isDockerRunning();
};

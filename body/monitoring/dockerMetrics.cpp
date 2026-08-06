#include "dockerMetrics.hpp"

#include <cpr/cpr.h>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

void DockerChecker::setContainersResponses(const std::string &urlToGet) {
  containersResponses.clear();
  for (auto &candidate : candidates) {
    cpr::Response response = cpr::Get(
        cpr::Url{urlToGet}, cpr::UnixSocket{candidate}, cpr::Timeout{1000});
    if (response.status_code != 200) {
      continue;
    }
    nlohmann::json containerResponse =
        nlohmann::json::parse(response.text, nullptr, false);
    if (containerResponse.is_discarded() || !containerResponse.is_array()) {
      continue;
    }
    containersResponses.push_back(containerResponse);
  }
}

DockerChecker::DockerChecker()
    : HOME_DIRECTORY(std::getenv("HOME") ? std::getenv("HOME") : ""),
      DOCKER_HOST(std::getenv("DOCKER_HOST") ? std::getenv("DOCKER_HOST")
                                             : "") {
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

void DockerChecker::addCandidateByPath(const std::string &pathToCheck) {
  if (!pathToCheck.empty()) {
    if (pathToCheck == HOME_DIRECTORY) {
      std::string candidateFromHome = HOME_DIRECTORY + MAC_PATH_TO_SOCKET;
      addToCandidates(candidateFromHome);
      return;
    }
    addToCandidates(pathToCheck);
  }
}

void DockerChecker::addToCandidates(const std::string &candidateToAdd) {
  if (isPrefixInPath(candidateToAdd)) {
    candidates.push_back(candidateToAdd.substr(PREFIX.size()));
  }
  candidates.push_back(candidateToAdd);
}

bool DockerChecker::isPrefixInPath(const std::string &candidate) {
  return candidate.rfind(PREFIX, 0) == 0;
}

bool DockerChecker::findDockerSocket() {
  for (const std::string &candidate : candidates) {
    if (pingDocker(candidate)) {
      return true;
    }
  }
  return false;
}

bool DockerChecker::pingDocker(const std::string &candidatePath) {
  cpr::Response response =
      cpr::Get(cpr::Url{DOCKER_PING_URL}, cpr::UnixSocket{candidatePath},
               cpr::Timeout{1000});

  return response.status_code == 200;
}

int DockerChecker::getCountRunningContainers() {
  setContainersResponses(DOCKER_NUM_CONT_URL);
  if (!containersResponses.empty()) {
    int runningCount = static_cast<int>(containersResponses.front().size());
    return runningCount;
  }
  return -1;
}

std::string DockerChecker::listContainers() {
  setContainersResponses(DOCKER_LIST_URL);
  if (containersResponses.empty())
    return "no containers";
  std::string finalString;
  for (const auto &response : containersResponses.front()) {
    std::string name = response["Names"][0].get<std::string>();
    std::string state = response["State"].get<std::string>();
    std::string status = response["Status"].get<std::string>();
    finalString += name + ": " + state + " (" + status + ")\n";
  }
  return finalString.empty() ? "no containers" : finalString;
}

std::string getContainersList() {
  DockerChecker checker;
  return checker.listContainers();
}

std::string checkIfDockerIsRunning() {
  DockerChecker checker;
  std::string dockerStatusText = std::string("docker is running: ") +
                                 (checker.isDockerRunning() ? "true" : "false");
  return dockerStatusText;
}

std::string getNumOfRunningContainers() {
  DockerChecker checker;
  std::string dockerCountText =
      "docker running containers count: " +
      std::to_string(checker.getCountRunningContainers());
  return dockerCountText;
}

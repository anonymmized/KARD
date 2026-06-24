#pragma once

#include <filesystem>
#include <string>

int runBinaryFileUpdate();
std::filesystem::path createTempDirectory();
std::string getPathToSelf();
int executeCommand(const std::string& command);

#pragma once

#include <string>
#include <fstream>

std::string summarizeHealth(const int seconds);
int appendSnapshot(const std::string& tool, const std::string& value);
std::ifstream openLogFile();

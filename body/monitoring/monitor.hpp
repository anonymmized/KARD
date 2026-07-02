#pragma once
#include "hostMetrics.hpp"
#include "dockerMetrics.hpp"

#include <string>

double computeUsage(const CpuTimes& prev, const CpuTimes& curr);
double getCpuUsage(int delayMs);
double getRamUsage();
std::string getDiskSpace();
std::string getAllMetrics();
double getTcpProbe(const char* destIp, uint16_t destPort);
std::string summarizeHealth(const int seconds);

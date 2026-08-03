#pragma once
#include "hostMetrics.hpp"
#include "dockerMetrics.hpp"

#include <string>

double computeCpuUsage(const CpuTimes& prev, const CpuTimes& curr);
double getCpuUsage(int delayMs = 200);
double getRamUsage();
std::string getDiskSpace();
std::string getAllMetrics();
double getTcpProbe(const char* destIp, uint16_t destPort);

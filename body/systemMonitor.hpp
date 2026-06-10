#pragma once

struct CpuTimes {
    unsigned long long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0;
};

bool readCpuTimes(CpuTimes &t);
double computeUsage(const CpuTimes& prev, const CpuTimes& curr);
double getCpuUsage(int delay_ms = 200);

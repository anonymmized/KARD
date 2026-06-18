#include "body/snapshot_log.hpp"
#include "body/systemMonitor.hpp"

int main() {
    appendSnapshot("get_cpu", std::to_string(getCpuUsage()));
    appendSnapshot("get_ram", std::to_string(getRamUsage()));
    appendSnapshot("get_temp", std::to_string(getTemp()));
    return 0;
}

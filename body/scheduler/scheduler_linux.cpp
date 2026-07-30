#include "body/scheduler/scheduler.hpp"
#include "body/scheduler/scheduler_help.hpp"

#include <string>
#include <filesystem>
#include <optional>
#include <iostream>

#ifndef SNAPSHOT_BIN
#define SNAPSHOT_BIN "kard-snapshot"
#endif

namespace {
    const std::string TIMER = "kard-snapshot.timer";
    const std::string SERVICE = "kard-snapshot.service";

    std::string getUnitDir(const std::string& home) {
        return std::string(home) + "/.config/systemd/user";
    }

    std::string getSvcPath(const std::string& home) {
        return getUnitDir(home) + "/" + SERVICE;
    }

    std::string getTimerPath(const std::string& home) {
        return getUnitDir(home) + "/" + TIMER;
    }

    bool reloadDaemon() {
        return scheduler::detail::run("systemctl --user daemon-reload");
    }

    bool enableDaemon() {
        return scheduler::detail::run("systemctl --user enable --now " + TIMER);
    }

    bool disableDaemon() {
        return scheduler::detail::run("systemctl --user disable --now " + TIMER + " 2>/dev/null");
    }

    bool checkIfDaemonRunning() {
        return scheduler::detail::run("systemctl --user is-active --quiet " + TIMER);
    }

    std::string getServiceBody() {
        return
            "[Unit]\n"
            "Description=KARD snapshot collector\n"
            "[Service]\n"
            "Type=oneshot\n"
            "ExecStart=" SNAPSHOT_BIN "\n";
    }

    std::string getTimerBody() {
        return
            "[Unit]\n"
            "Description=KARD snapshot every 10 min\n"
            "[Timer]\n"
            "OnBootSec=1min\n"
            "OnUnitActiveSec=600\n"
            "[Install]\n"
            "WantedBy=timers.target\n";
    }
}

bool installSnapshotJob() {
    std::optional<std::string> homePath = scheduler::detail::resolveHomeDir();
    if (!homePath) {
        return false;
    }

    std::error_code errorCode;
    std::filesystem::create_directories(getUnitDir(*homePath), errorCode);
    bool svcWriteResult = scheduler::detail::writeFile(getSvcPath(*homePath), getServiceBody());
    bool timerWriteResult = scheduler::detail::writeFile(getTimerPath(*homePath), getTimerBody());
    if (!svcWriteResult || !timerWriteResult) {
        std::cerr << "scheduler: failed to write systemd units\n";
        return false;
    }

    reloadDaemon();
    return enableDaemon();
}

bool uninstallSnapshotJob() {
    std::optional<std::string> homePath = scheduler::detail::resolveHomeDir();
    if (!homePath) {
        return false;
    }

    disableDaemon();
    std::error_code errorCode;
    std::filesystem::remove(getSvcPath(*homePath), errorCode);
    std::filesystem::remove(getTimerPath(*homePath), errorCode);
    reloadDaemon();
    return true;
}

bool isSnapshotJobInstalled() {
    return checkIfDaemonRunning();
}

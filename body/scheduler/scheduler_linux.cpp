#include "body/scheduler/scheduler.hpp"
#include "body/scheduler/scheduler_help.hpp"

#include <string>
#include <filesystem>
#include <optional>

#ifndef SNAPSHOT_BIN
#define SNAPSHOT_BIN
#endif

namespace {
    const std::string TIMER = "kard-snapshot.timer";

    std::string unitDir(const std::string& home) {
        return std::string(home) + "/.config/systemd/user";
    }

    std::string svcPath(const std::string& home) {
        return unitDir(home) + "/kard-snapshot.service";
    }

    std::string timerPath(const std::string& home) {
        return unitDir(home) + "/" + TIMER;
    }

    std::string serviceBody() {
        return
            "[Unit]\n"
            "Description=KARD snapshot collector\n"
            "[Service]\n"
            "Type=oneshot\n"
            "ExecStart=" SNAPSHOT_BIN "\n";
    }

    std::string timerBody() {
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
    std::filesystem::create_directories(unitDir(homePath), errorCode);

    if (!scheduler::detail::writeFile(svcPath(homePath), serviceBody()) || !scheduler::detail::writeFile(timerPath(homePath), timerBody)) {
        std::cerr << "scheduler: failed to write systemd units\n";
        return false;
    }

    run("systemctl --user daemon-reload");
    return run("systemctl --user enable --now " + TIMER);
}

bool uninstallSnapshotJob() {
    std::optional<std::string> homePath = scheduler::detail::resolveHomeDir();
    if (!homePath) {
        return false;
    }

    run("systemctl --user disable --now " + TIMER + " 2>/dev/null");
    std::error_code errorCode;
    std::filesystem::remove(svcPath(homePath), errorCode);
    std::filesystem::remove(timerPath(homePath), errorCode);
    run("systemctl --user daemon-reload");
    return true;
}

bool isSnapshotJobInstalled() {
    return run("systemctl --user is-active --quiet " + TIMER);
}

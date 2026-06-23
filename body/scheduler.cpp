#include "body/scheduler.hpp"

#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#ifndef SNAPSHOT_BIN
#define SNAPSHOT_BIN "kard-snapshot"
#endif

namespace {
    bool run(const std::string& cmd) {
        return std::system(cmd.c_str()) == 0;
    }

    bool writeFile(const std::string& path, const std::string& textToWrite) {
        std::ofstream outFile(path, std::ios::trunc);
        if (!outFile.is_open()) return false;
        outFile << textToWrite;
        return true;
    }

    const char* homeDir() {
        return std::getenv("HOME");
    }
}

#ifdef __APPLE__

namespace {
    const std::string LABEL = "com.kard.snapshot";

    std::string userDomain() {
        return "gui/" + std::to_string(getuid());
    }

    std::string plistPath(const char* home) {
        return std::string(home) + "/Library/LaunchAgents/" + LABEL + ".plist";
    }

    std::string plistBody() {
        return
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
            "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            "<plist version=\"1.0\">\n"
            "<dict>\n"
            "    <key>Label</key>\n"
            "    <string>" + LABEL + "</string>\n"
            "    <key>ProgramArguments</key>\n"
            "    <array>\n"
            "        <string>" SNAPSHOT_BIN "</string>\n"
            "    </array>\n"
            "    <key>StartInterval</key>\n"
            "    <integer>600</integer>\n"
            "    <key>RunAtLoad</key>\n"
            "    <true/>\n"
            "    <key>StandardErrorPath</key>\n"
            "    <string>/tmp/kard-snapshot.err</string>\n"
            "</dict>\n"
            "</plist>\n";
    }

}

bool installSnapshotJob() {
    const char* home = homeDir();
    if (!home) {
        std::cerr << "scheduler: $HOME is not set\n"; return false;
    }

    std::error_code ec;
    std::filesystem::create_directories(std::string(home) + "/Library/LaunchAgents", ec);

    if (!writeFile(plistPath(home), plistBody())) {
        std::cerr << "scheduler: failed to write plist\n";
        return false;
    }

    run("launchctl bootout " + userDomain() + "/" + LABEL + " 2>/dev/null");
    return run("launchctl bootstrap " + userDomain() + " " + plistPath(home));
}

bool uninstallSnapshotJob() {
    const char* home = homeDir();
    if (!home) {
        std::cerr << "scheduler: $HOME is not set\n"; return false;
    }

    run("launchctl bootout " + userDomain() + "/" + LABEL + " 2>/dev/null");
    std::error_code ec;
    std::filesystem::remove(plistPath(home), ec);
    return true;
}

bool isSnapshotJobInstalled() {
    return run("launchctl print " + userDomain() + "/" + LABEL + " >/dev/null 2>&1");
}

#else

namespace {
    const std::string TIMER = "kard-snapshot.timer";

    std::string unitDir(const char* home) {
        return std::string(home) + "/.config/systemd/user";
    }

    std::string svcPath(const char* home) {
        return unitDir(home) + "/kard-snapshot.service";
    }

    std::string timerPath(const char* home) {
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
    const char* home = homeDir();
    if (!home) {
        std::cerr << "scheduler: $HOME is not set\n"; return false;
    }

    std::error_code ec;
    std::filesystem::create_directories(unitDir(home), ec);

    if (!writeFile(svcPath(home), serviceBody()) || !writeFile(timerPath(home), timerBody())) {
        std::cerr << "scheduler: failed to write systemd units\n";
        return false;
    }

    run("systemctl --user daemon-reload");
    return run("systemctl --user enable --now " + TIMER);
}

bool uninstallSnapshotJob() {
    const char* home = homeDir();
    if (!home) {
        std::cerr << "scheduler: $HOME is not set\n"; return false;
    }

    run("systemctl --user disable --now " + TIMER + " 2>/dev/null");
    std::error_code ec;
    std::filesystem::remove(svcPath(home), ec);
    std::filesystem::remove(timerPath(home), ec);
    run("systemctl --user daemon-reload");
    return true;
}

bool isSnapshotJobInstalled() {
    return run("systemctl --user is-active --quiet " + TIMER);
}

#endif

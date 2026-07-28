#include "body/scheduler/scheduler.hpp"
#include "body/scheduler/scheduler_help.hpp"

#include <string>
#include <filesystem>
#include <optional>
#include <unistd.h>
#include <iostream>

#ifndef SNAPSHOT_BIN
#define SNAPSHOT_BIN "kard-snapshot"
#endif

namespace {
    const std::string LABEL = "com.kard.snapshot";
    const std::string PATH_TO_AGENTS = "/Library/LaunchAgents/";

    std::string getUserDomain() {
        return "gui/" + std::to_string(getuid());
    }

    std::string getPlistPath(const std::string& homePath) {
        return std::string(homePath) + PATH_TO_AGENTS + LABEL + ".plist";
    }

    bool unloadJob() {
        return scheduler::detail::run("launchctl bootout " + getUserDomain() + "/" + LABEL + " 2>/dev/null");
    }

    bool loadJob(const std::string& plistPath) {
        return scheduler::detail::run("launchctl bootstrap " + getUserDomain() + " " + plistPath);
    }

    bool isJobLoaded() {
        return scheduler::detail::run("launchctl print " + getUserDomain() + "/" + LABEL + " >/dev/null 2>&1");
    }

    std::string getPlistBody() {
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
    std::optional<std::string> homePath = scheduler::detail::resolveHomeDir();
    if (!homePath) {
        return false;
    }

    std::error_code errorCode;
    std::filesystem::create_directories(*homePath + PATH_TO_AGENTS, errorCode);

    std::string plistBody = getPlistBody();
    std::string plistPath = getPlistPath(*homePath);
    if (!scheduler::detail::writeFile(plistPath, plistBody)) {
        std::cerr << "scheduler: failed to write plist\n";
        return false;
    }

    unloadJob();
    return loadJob(plistPath);
}

bool uninstallSnapshotJob() {
    std::optional<std::string> homePath = scheduler::detail::resolveHomeDir();
    if (!homePath) {
        return false;
    }
    unloadJob();

    std::error_code errorCode;
    std::string plistPath = getPlistPath(*homePath);
    std::filesystem::remove(plistPath, errorCode);
    return true;
}

bool isSnapshotJobInstalled() {
    return isJobLoaded();
}

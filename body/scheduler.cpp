#include "body/scheduler.hpp"

#include <string>
#include <fstream>
#include <filesystem>

#ifndef SNAPSHOT_BIN
#define SNAPSHOT_BIN "kard-snapshot"
#endif

namespace {
bool run(const std::string& cmd) {
    return std::system(cmd.c_str()) == 0;
}

bool writeFile(const std::string& path, const std::string& text) {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return false;
    out << text;
    return true;
}

const char* homeDir() { return std::getenv("HOME"); }

}

#ifdef __APPLE__

namespace {
const std::string LABEL = "com.kard.snapshot";

std::string userDomain() { return "gui/" + std::to_string(getuid()); }

std::string plistPath(const char* home) { return std::string(home) + "/Library/LaunchAgents/" + LABEL + ".plist"; }

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
    if (!home) { std::cerr << "scheduler: $HOME is not set\n"; return false; }

    std::error_code ec;
    std::filesystem::create_directories(std::string(home) + "/Library/LaunchAgents", ec);

    if (!writeFile(plistPath(home), plistBody())) {
        std::cerr << "scheduler: failed to write plist\n";
        return false;
    }

    run("launchctl bootout " + userDomain() + "/" + LABEL + " 2>/dev/null");
    return run("launchctl bootstrap " + userDomain() + " " + plistPath(home));
}

boot uninstallSnapshotJob() {
    const char* home = homeDir();
    if (!home) { std::cerr << "scheduler: $HOME is not set\n"; return false; }

    run("launchctl bootout " + userDomain() + "/" + LABEL + " 2>/dev/null");
    std::error_code ec;
    std::filesystem::remove(plistPath(home), ec);
    return true;
}

bool isSnapshotJobInstalled() {
    return run("launchctl print " + userDomain() + "/" + LABEL + " >/dev/null 2>&1");
}

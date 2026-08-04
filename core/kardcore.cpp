#include "kardcore.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "body/scheduler/scheduler.hpp"
#include "body/paths.hpp"
#include "voice/voice.hpp"
#include "selfupdate/selfUpdate.hpp"
#include "core/argsParse.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

constexpr int RUN_CHAT = 2;

void KardCore::run() {
    while (true) {
        std::string userText = interfaces.input.read();
        if (userText == "/exit") {
            return;
        }
        interfaces.output.startThinking();
        std::string ans = interfaces.brain.ask(userText);
        interfaces.output.stopThinking();
        interfaces.output.show(ans);
    }
}

int executeArgs(CliArguments arguments) {
    switch (arguments.mode) {
        case Mode::StartDaemon:  return startDaemon();
        case Mode::StopDaemon:   return stopDaemon();
        case Mode::DaemonStatus: return getDaemonStatus();
        case Mode::Update:       return getBasicUpdate();
        case Mode::FullUpdate:   return getFullUpdate();
        case Mode::Test:         testArgument(); return 0;
        case Mode::Help:         printHelpPage(); return 0;
        case Mode::Unknown:
            std::cout << "There is no argument like this\n";
            printHelpPage();
            return 0;
        case Mode::Repl:         return RUN_CHAT;
    }
    return 0;
}

int startDaemon() {
    if (!installSnapshotJob()) {
        std::cerr << "The daemon wasn't started\n";
        return 1;
    }
    std::cout << "The daemon was started\n";
    return 0;
}

int stopDaemon() {
    if (!uninstallSnapshotJob()) {
        std::cerr << "The daemon wasn't uninstall\n";
        return 1;
    }
    std::cout << "The daemon was uninstall\n";
    return 0;
}

int getDaemonStatus() {
    if (!isSnapshotJobInstalled()) {
        std::cout << "The daemon is not running\n";
        return 0;
    }
    std::cout << "The daemon is running\n";
    return 0;
}

int getBasicUpdate() {
    try {
        Updater updater;
        if (updater.runBinaryFileUpdate() != 0) {
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Update failed: " << e.what() << '\n';
        return 1;
    }
    std::cout << "The program updater successfully\n";
    return 0;
}

int getFullUpdate() {
    try {
        Updater updater;
        if (updater.runFullUpdate() != 0) {
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Update failed: " << e.what() << '\n';
        return 1;
    }
    std::cout << "The project updated successfully\n";;
    return 0;
}

void testArgument() {
    std::cout << "This is a test for program\n";
}

void printHelpPage() {
    std::cout << "This is a page for help\n";
}

ReplInterfaces bindInterfaces(ReplComponents& components) {
    return { components.terminalInput,
             components.output,
             components.brain
    };
}

int main(int argc, char** argv) {
    ArgumentParser parser(argc, argv);
    CliArguments args = parser.parseArguments();
    int exitCode = executeArgs(args);
    if (exitCode != RUN_CHAT) {
        return exitCode;
    }

    OllamaBrain ollama;
    TerminalOutput terminalOutput;
    VoiceOutput voiceOutput;
    CompositeOutput compositeOutput(terminalOutput, voiceOutput);
    IOutput& output = args.voice
        ? static_cast<IOutput&>(compositeOutput)
        : static_cast<IOutput&>(terminalOutput);
    ReplComponents components(ollama, output);
    ReplInterfaces mainInterfaces = bindInterfaces(components);
    std::cout << "user: ";
    KardCore core(mainInterfaces);
    core.run();
    return 0;
}

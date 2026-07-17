#include "kardcore.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "body/scheduler.hpp"
#include "body/paths.hpp"
#include "voice/voice.hpp"
#include "selfupdate/selfUpdate.hpp"
#include "core/argsParse.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

void KardCore::run() {
    while (true) {
        std::string userText = input.read();
        if (userText == "/exit") {
            return;
        }
        output.startThinking();
        std::string ans = brain.ask(userText);
        output.stopThinking();
        output.show(ans);
    }
}

int executeArgs(CliArguments arguments) {
    int exitCode = 0;
    if (arguments.mode == Mode::StartDaemon) {
        exitCode = startDaemon();
    }
    if (arguments.mode == Mode::StopDaemon) {
        exitCode = stopDaemon();
    }
    if (arguments.mode == Mode::DaemonStatus) {
        exitCode = getDaemonStatus();
    }
    if (arguments.mode == Mode::Update) {
        exitCode = getBasicUpdate();
    }
    if (arguments.mode == Mode::FullUpdate) {
        exitCode = getFullUpdate();
    }
    if (arguments.mode == Mode::Test) {
        testArgument();
    }
    if (arguments.mode == Mode::Help) {
        printHelpPage();
    }
    if (arguments.mode == Mode::Unknown) {
        std::cout << "There is no argument like this\n";
        printHelpPage();
    }
    if (arguments.mode == Mode::Repl) {
        exitCode = 2;
    }
    return exitCode;
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



int main(int argc, char** argv) {
    ArgumentParser parser(argc, argv);
    CliArguments args = parser.parseArguments();
    int exitCode = executeArgs(args);
    if (exitCode != 2) {
        return 0;
    }
    std::vector<nlohmann::json> base;
    TerminalInput t_input;
    VoiceOutput v_output;
    TerminalOutput t_output;
    CompositeOutput c_output(t_output, v_output);
    OllamaBrain ollama(base);
    IInput& input = t_input;
    IOutput& output = c_output;
    IBrain& brain = ollama;
    std::cout << "user: ";
    KardCore core(input, output, brain);
    core.run();
    return 1;
}

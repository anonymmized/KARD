#include "kardcore.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "body/scheduler.hpp"
#include "body/paths.hpp"
#include "voice/voice.hpp"
#include "selfupdate/selfUpdate.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

void KardCore::run() {
    while (true) {
        std::string req = input.read();
        if (req == "/exit") return;
        output.startThinking();
        std::string ans = brain.ask(req);

        output.stopThinking();
        output.show(ans);
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        std::string cmd = argv[1];
        if (cmd == "--rund") {
            if (!installSnapshotJob()) {
                std::cerr << "The daemon wasn't started\n";
                return 1;
            }
            std::cout << "The daemon was started with no problems\n";
            return 0;
        }
        if (cmd == "--stopd") {
            if (!uninstallSnapshotJob()) {
                std::cerr << "The daemon wasn't uninstall\n";
                return 1;
            }
            std::cout << "The daemon was uninstall with no problems\n";
            return 0;
        }
        if (cmd == "--statusd") {
            if (!isSnapshotJobInstalled()) {
                std::cout << "The daemon is not running\n";
                return 0;
            }
            std::cout << "The daemon is running\n";
            return 0;
        }
        if (cmd == "--update") {
            try {
                Updater updater;
                if (updater.runBinaryFileUpdate() != 0) {
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "Update failed: " << e.what() << '\n';
                return 1;
            }
            std::cout << "The program updated successfully\n";
            return 0;
        }
        if (cmd == "--full-update") {
            try {
                Updater updater;
                if (updater.runFullUpdate() != 0) {
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "Update failed: " << e.what() << '\n';
                return 1;
            }
            std::cout << "The project updated successfully\n";
            return 0;
        }
        if (cmd == "--test") {
            std::cout << "This is a test for full update\n";
            return 0;
        }
        else {
            std::cout << "There is no argument like this\n";
            return 1;
        }
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

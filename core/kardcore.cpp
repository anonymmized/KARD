#include "kardcore.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "body/scheduler.hpp"

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
            if (!installSnapshotJob()) { std::cout << "The daemon wasn't started\n"; return 1; }
            std::cout << "The daemon was started with no problems\n";
            return 0;
        }
        if (cmd == "--stopd") {
            if (!uninstallSnapshotJob()) { std::cout << "The daemon wasn't uninstall\n"; return 1;}
            std::cout << "The daemon was uninstall with no problems\n";
            return 0;
        }
        if (cmd == "--statusd") {
            if (!isSnapshotJobInstalled()) { std::cout << "The daemon is not running\n"; return 0; }
            std::cout << "The daemon is running\n";
            return 0;
        }
        else {
            std::cout << "There is no argument like this\n";
            return 1;
        }
    }
    std::vector<nlohmann::json> base;
    TerminalInput t_input;
    TerminalOutput t_output;
    OllamaBrain ollama(base);
    IInput& input = t_input;
    IOutput& output = t_output;
    IBrain& brain = ollama;
    std::cout << "user: ";
    KardCore core(input, output, brain);
    core.run();
    return 1;
}

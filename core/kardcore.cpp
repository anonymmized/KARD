#include "kardcore.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

void KardCore::run() {
    while (true) {
        std::string req = input.read();
        if (req == "/exit") return;
        std::string ans = brain.ask(req);
        output.show(ans);
    }
}

int main() {
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

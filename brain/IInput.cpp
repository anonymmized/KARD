#include "IInput.hpp"

#include <iostream>

std::string TerminalInput::read() {
    int inputRow = setup.getInputRow();
    std::cout << "\033[" << inputRow << ";1H" << "\033[2K" << "> " << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) return "/exit";
    return line;
}

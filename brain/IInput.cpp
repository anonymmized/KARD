#include "IInput.hpp"

#include <iostream>

std::string TerminalInput::read() {
    std::string line;
    if (!std::getline(std::cin, line)) return "/exit";
    return line;
}

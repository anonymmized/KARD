#include "IInput.hpp"

#include <iostream>

std::string TerminalInput::read() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

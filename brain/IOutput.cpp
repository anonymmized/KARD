#include "IOutput.hpp"

#include <iostream>
#include <string>

void TerminalOutput::show(const std::string& text) {
    std::cout << "Assistant: " << text << '\n' << "user: ";
}

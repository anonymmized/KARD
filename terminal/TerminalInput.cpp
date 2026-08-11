#include "terminal/TerminalInput.hpp"
#include "terminal/RawMode.hpp"

#include <iostream>

std::string TerminalInput::read() {
    RawMode rawMode;
    std::string line;
    redraw(line);

    while (true) {
        char symbol;
        if (::read(STDIN_FILENO, &symbol, 1) != 1) {
            continue;
        }
        if (symbol == '\n' || symbol == '\r') {
            break;
        }
        if (symbol == 127 || symbol == 8) {
            if (!line.empty()) {
                while (line.size() > 1 && ((unsigned char)line.back() & 0xC0) == 0x80) {
                    line.pop_back();
                }
                line.pop_back();
            }
        } else if ((unsigned char)symbol >= 32) {
            line += symbol;
        }
        redraw(line);
    }
    return line;
}

void TerminalInput::redraw(const std::string& line) {
    int row = setup.getInputRow();
    int cols = setup.getCols();
    int visible = cols - 4;
    std::string tail = (int)line.size() > visible ? line.substr(line.size() - visible) : line;
    std::cout << "\033[" << row << ";1H\033[2K" << "> " << tail << std::flush;
}

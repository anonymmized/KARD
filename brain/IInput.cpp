#include "IInput.hpp"
#include "rawGuard.hpp"

#include <iostream>

void TerminalInput::redraw(const std::string& line) {
    int row = setup.getInputRow();
    int cols = setup.getCols();
    int visible = cols - 4;
    std::string tail = (int)line.size() > visible ? line.substr(line.size() - visible) : line;
    std::cout << "\033[" << row << ";1H\033[2K" << "> " << tail << std::flush;
}

std::string TerminalInput::read() {
    RawMode raw;
    std::string line;
    redraw(line);

    while (true) {
        char chr;
        if (::read(STDIN_FILENO, &chr, 1) != 1) {
            continue;
        }
        if (chr == '\n' || chr == '\r') {
            break;
        }
        if (chr == 127 || chr == 8) {
            if (!line.empty()) {
                while (line.size() > 1 && ((unsigned char)line.back() & 0xC0) == 0x80) {
                    line.pop_back();
                }
                line.pop_back();
            }
        } else if ((unsigned char)chr >= 32) {
            line += chr;
        }
        redraw(line);
    }
    return line;
}

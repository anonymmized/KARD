#include "core/terminal.hpp"

#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

/*
 * 1) In constructor must be terminal binding: split on two zones (user's input and model's output) 
 * 2) Terminal clearing
*/

void TerminalSetup::drawLine(int row) {
    std::cout << "\033[" << row << ";1H";
    for (int i = 0; i < termSize.cols; i++) {
        std::cout << "─";
    }
}

void TerminalSetup::setTerminalSize() {
    winsize winsz{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz) == 1) {
        return;
    }
    termSize.cols = winsz.ws_col;
    termSize.rows = winsz.ws_row;
}

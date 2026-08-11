#include "terminal/terminaSetup.hpp"

#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>

TerminalSetup::TerminalSetup() {
    setTerminalSize();
    std::cout << "\033[2J";
    std::cout << "\033[1;" << getOutputBottom() << "r";
    drawLine(termSize.rows - 2);
    drawLine(termSize.rows);
    std::cout << "\033[1;1H" << std::flush;
}

TerminalSetup::~TerminalSetup() {
    std::cout << "\033[r" << "\033[?25h" << "\033[2J\033[H" << std::flush;
}

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

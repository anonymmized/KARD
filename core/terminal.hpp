#pragma once

#include <iostream>

struct TermSize {
    int cols = 80;
    int rows = 24;
};

class TerminalSetup {
    private:
        TermSize termSize;
        int reservedToInput;
        void drawLine(int row);
    public:
        TerminalSetup(int _reservedToInput = 3) : reservedToInput(_reservedToInput) {
            setTerminalSize();
            std::cout << "\033[2J";
            std::cout << "\033[1;" << getOutputBottom() << "r";
            drawLine(termSize.rows - 2);
            drawLine(termSize.rows);
            std::cout << "\033[1;1H" << std::flush;
        }
        ~TerminalSetup() {
            std::cout << "\033[r" << "\033[?25h" << "\033[2J\033[H" << std::flush;
        }
        void setTerminalSize();
        int getOutputBottom() { return termSize.rows - reservedToInput; }
        int getInputRow() { return termSize.rows - 1; }
        int getCols() { return termSize.cols; }
};

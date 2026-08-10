#pragma once

#include <iostream>

struct TermSize {
    int cols;
    int rows;
};

class TerminalSetup {
    private:
        TermSize termSize;
        int reservedToInput = 3;
    public:
        TerminalSetup(int _reservedToInput = 3) : reservedToInput(_reservedToInput) {
            setTerminalSize();
            std::cout << "\033[2J";
            std::cout << "\033[1;" << getOutputBottom() << "r";
            std::cout << "\033[1;1H" << std::flush;
        }
        ~TerminalSetup() = default;
        void setTerminalSize();
        int getOutputBottom() { return termSize.rows - reservedToInput; }
        int getInputRow() { return termSize.rows - 1; }
};

#pragma once

struct TermSize {
    int cols = 80;
    int rows = 24;
};

class TerminalSetup {
    private:
        TermSize termSize;
        const RESERVED_LINES = 3;
        void drawLine(int row);
    public:
        TerminalSetup();
        ~TerminalSetup();
        void setTerminalSize();
        int getOutputBottom() { return termSize.rows - RESERVED_LINES; }
        int getInputRow() { return termSize.rows - 1; }
        int getCols() { return termSize.cols; }
};

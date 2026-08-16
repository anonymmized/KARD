#pragma once

#include <string>

struct TermSize {
    int cols = 80;
    int rows = 24;
};

class TerminalSetup {
    private:
        TermSize termSize;
        const int RESERVED_LINES = 3;
        void drawLine(int row);
        bool interactive = false;
        std::string reset;
        std::string grey;
        std::string greyBackground;
    public:
        TerminalSetup();
        ~TerminalSetup();
        void setTerminalSize();
        int getOutputBottom() { return termSize.rows - RESERVED_LINES; }
        int getInputRow() { return termSize.rows - 1; }
        int getCols() { return termSize.cols; }
        const std::string getGrey() const { return grey; }
        const std::string getGreyBackground() const { return greyBackground; }
        const std::string getReset() const { return reset; }
        bool isInteractive() { return interactive; }
};

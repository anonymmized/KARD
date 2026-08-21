#include "MainController.hpp"

TermSize Render::getTermSize() {
    termSize termSize;
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        termSize.cols = ws.ws_col;
        termSize.rows = ws.ws_row;
    } else {
        perror("ioctl");
    }
    return termSize;
}

std::vector<std::string> Render::wrapText(const std::string& targetText, int width) {
    std::vector<std::string> wrappedText;
    std::string line;
    for (char symbol : targetText) {
        if (line.size() >= width - 1) {
            line += symbol;
            wrappedText.push_back(line);
            line.clear();
            continue;
        }
        line += symbol;
        if (symbol == '\n' || symbol == '\r') {
            wrappedText.push_back(line);
            line.clear();
        }
    }
    if (!line.empty()) {
        wrappedText.push_back(line);
    }
    return wrappedText;
}

int Render::calculateOutputHeight(int termHeight) {
    return termHeight - NUM_OF_RESERVED_LINES;
}

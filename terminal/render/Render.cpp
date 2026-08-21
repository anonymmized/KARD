#include "Render.hpp"
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>

Render::TermSize Render::getTermSize() {
    Render::TermSize termSize;
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

int Render::maxFirstVisible() {
    int linesCount = static_cast<int>(state.wrappedLines.size());
    if (linesCount <= state.outputHeight) {
        return 0;
    }
    return linesCount - state.outputHeight;
}

void Render::clampViewport() {
    if (state.firstVisible < 0) {
        state.firstVisible = 0;
    }
    int maxFirst = maxFirstVisible();
    if (state.firstVisible > maxFirst) {
        state.firstVisible = maxFirst;
    }
}

void Render::scrollToBottom() {
    state.firstVisible = maxFirstVisible();
}

void Render::render() {
    for (int i = 0; i < state.outputHeight; i++) {
        int lineIndex = state.firstVisible + i;
        std::cout << "\033[" << i + 1 << ";1H";
        std::cout << "\033[2K";
        if (lineIndex < state.wrappedLines.size()) {
            std::cout << state.wrappedLines[lineIndex] << std::flush;
        }
    }
}

void Render::appendText(const std::string& textToAppend) {
    allText += textToAppend;
    state.wrappedLines = wrapText(allText, termSize.cols);
    state.outputHeight = calculateOutputHeight(termSize.rows);
    scrollToBottom();
    render();
}

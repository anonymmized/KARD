#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include "../terminal/RawMode.hpp"
#include "text.hpp"

struct TermSize {
    int cols = 80;
    int rows = 24;
};

struct State {
    std::vector<std::string> wrappedLines;
    int firstVisible = 0;
    int outputHeight = 0;
};


int maxFirstVisible(const State& state) {
    int linesCount = static_cast<int>(state.wrappedLines.size());
    if (linesCount <= state.outputHeight) {
        return 0;
    }
    return linesCount - state.outputHeight;
}

void clampViewport(State& state) {
    if (state.firstVisible < 0) {
        state.firstVisible = 0;
    }
    int maxFirst = maxFirstVisible(state);
    if (state.firstVisible > maxFirst) {
        state.firstVisible = maxFirst;
    }
}

void scrollUp(State& state, int lines) {
    state.firstVisible -= lines;
    clampViewport(state);
}

void scrollDown(State& state, int lines) { 
    state.firstVisible += lines;
    clampViewport(state);
}

void render(const State& state) {
    for (int i = 0; i < state.outputHeight; i++) {
        int lineIndex = state.firstVisible + i;
        std::cout << "\033[" << i + 1 << ";1H";
        std::cout << "\033[2K";
        if (lineIndex < state.wrappedLines.size()) {
            std::cout << state.wrappedLines[lineIndex] << std::flush;
        }
    }
}

TermSize getTermSize() {
    TermSize termSize;
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        termSize.cols = ws.ws_col;
        termSize.rows = ws.ws_row;
    } else {
        perror("ioctl");
    }
    return termSize;
}

std::vector<std::string> splitInput(const std::string& text, int width) {
    std::vector<std::string> wrappedLines;
    std::string line;
    for (char symbol : text) {
        if (line.size() >= width - 1) {
            line += symbol;
            wrappedLines.push_back(line);
            line.clear();
            continue;
        }
        line += symbol;
        if (symbol == '\n' || symbol == '\r') {
            wrappedLines.push_back(line);
            line.clear();
        } 
    }
    if (!line.empty()) {
        wrappedLines.push_back(line);
    }
    return wrappedLines;
}

int calculateOutputHeight(int termHeight) {
    return termHeight - 3;
}

int main() {
    RawMode rawMode;
    TermSize termSize = getTermSize();

    std::vector<std::string> wrapped = splitInput(TEXT_TO_INPUT, termSize.cols);
    State state;
    state.wrappedLines = wrapped;

    int outputH = calculateOutputHeight(termSize.rows);
    state.outputHeight = outputH;
    render(state);

    while (true) {
        char symbol;
        if (read(STDIN_FILENO, &symbol, 1) != 1) {
            continue;
        }
        if (symbol == 113) {
            break;
        }
        if (symbol == 106) {
            scrollDown(state, 1);
        }
        if (symbol == 107) {
            scrollUp(state, 1);
        }
        render(state);
    }
    return 0;
}

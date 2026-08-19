#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include "../terminal/RawMode.hpp"
#include "text.hpp"

int firstVisible = 0;
int wrappedLinesCount = 0;
int outputHeight = 0;

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
    return state.wrappedLines.size() - state.outputHeight;
}

void clampViewport(State& state) {

}

void scrollUp(State& state, int lines) {

}

void scrollDown(State& state, int lines) {
    
}

void render(const State& state) {
    int visibleBegin = state.firstVisible;
    int visibleEnd   = min(state.firstVisible + state.outputHeight, wrappedLines.size());
    for (int i = visibleBegin; i < visibleEnd - 1; i++) {
        std::cout << state.text[i];
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

    std::vector<std::string> wrapped = splitInput(TEXT_TO_INPUT, temrSize.cols);
    State state.wrappedLines = wrapped;

    int outputH = calculateOutputHeight(termSize.rows);
    state.outputHeight = outputH;
    render;

    while (true) {
        get symbol from read();
        if 'q' exit;
        if 'j' scrollDown(3);
        if 'k' scrollUp(3);
        render;
    }
}

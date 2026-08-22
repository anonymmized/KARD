#include "Render.hpp"
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>

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

int Render::utf8CharLength(unsigned char lead) {
    if ((lead & 0x80) == 0x00) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

std::vector<std::string> Render::wrapText(const std::string& targetText, int width) {
    std::vector<std::string> wrappedText;
    std::string line;
    int lineWidth = 0;
    for (size_t charIndex = 0; charIndex < targetText.size();) {
        unsigned char lead = static_cast<unsigned char>(targetText[charIndex]);
        int charLen = utf8CharLength(lead);

        if (charIndex + charLen > targetText.size()) {
            charLen = 1;
        }

        std::string symbol = targetText.substr(charIndex, charLen);
        charIndex += charLen;

        if (symbol == "\n" || symbol == "\r") {
            wrappedText.push_back(line);
            line.clear();
            lineWidth = 0;
            continue;
        }

        if (lineWidth >= width - 1) {
            wrappedText.push_back(line);
            line.clear();
            lineWidth = 0;
        }

        line += symbol;
        lineWidth += 1;
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

void Render::render(RenderStates renderState) {
    for (int i = 0; i < state.outputHeight; i++) {
        int lineIndex = state.firstVisible + i;
        std::cout << "\033[" << i + 1 << ";1H";
        std::cout << "\033[2K";
        if (lineIndex < state.wrappedLines.size()) {
            if (renderState == RenderStates::Rewrite) {
                if (lineIndex >= oldWrappedCount) {
                    typewriteText(state.wrappedLines[lineIndex]);
                } else {
                    std::cout << state.wrappedLines[lineIndex] << std::flush;
                }
            } else if (renderState == RenderStates::Scroll) {
                std::cout << state.wrappedLines[lineIndex]<< std::flush;
            }
        }
    }
}

void Render::typewriteText(const std::string& textToWrite) {
    for (char symbol : textToWrite) {
        std::cout << symbol << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void Render::appendText(const std::string& textToAppend) {
    allText += textToAppend;
    oldWrappedCount = state.wrappedLines.size();
    state.wrappedLines = wrapText(allText, termSize.cols);
    state.outputHeight = calculateOutputHeight(termSize.rows);
    scrollToBottom();
    render(RenderStates::Rewrite);
}

void Render::scrollUp(int lines) {
    state.firstVisible -= lines;
    clampViewport();
    render(RenderStates::Scroll);
}

void Render::scrollDown(int lines) {
    state.firstVisible += lines;
    clampViewport();
    render(RenderStates::Scroll);
}

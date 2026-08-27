#include "terminal/render/Render.hpp"
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

void Render::render(RenderStates renderState) {
    for (int i = 0; i < scroller.getOutputHeight(); i++) {
        int lineIndex = scroller.getFirstVisible() + i;
        std::cout << "\033[" << i + 1 << ";1H";
        std::cout << "\033[2K";
        if (lineIndex < wrappedLines.size()) {
            if (renderState == RenderStates::Rewrite) {
                if (lineIndex >= oldWrappedCount) {
                    typewriteText(wrappedLines[lineIndex]);
                } else {
                    std::cout << wrappedLines[lineIndex] << std::flush;
                }
            } else if (renderState == RenderStates::Scroll) {
                std::cout << wrappedLines[lineIndex]<< std::flush;
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
    oldWrappedCount = wrappedLines.size();
    wrappedLines = wrapper.wrapText(allText, termSize.cols);
    scroller.setTermHeight(termSize.rows);
    scroller.calculateOutputHeight();
    scroller.scrollToBottom();
    render(Rewrite);
}

void Render::scrollUp(int lines) {
    scroller.up(lines);
    render(Scroll);
}

void Render::scrollDown(int lines) {
    scroller.down(lines);
    render(Scroll);
}

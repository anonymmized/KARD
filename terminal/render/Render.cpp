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
        viewer.moveCursor(i + 1);
        viewer.clearLine();
        if (lineIndex < wrappedLines.size()) {
            if (renderState == RenderStates::Rewrite) {
                if (lineIndex >= oldWrappedCount) {
                    viewer.typewriteText(wrappedLines[lineIndex]);
                } else {
                    std::cout << wrappedLines[lineIndex] << std::flush;
                }
            } else if (renderState == RenderStates::Scroll) {
                std::cout << wrappedLines[lineIndex]<< std::flush;
            }
        }
    }
}

void Render::appendText(const std::string& textToAppend) {
    allText += textToAppend;
    oldWrappedCount = wrappedLines.size();
    wrappedLines = wrapper.wrapText(allText, termSize.cols);
    scroller.setLinesCount(static_cast<int>(wrappedLines.size()));
    scroller.setTermHeight(termSize.rows);
    scroller.calculateOutputHeight();
    scroller.scrollToBottom();
    render(Rewrite);
}

void Render::scrollUp(int lines) {
    scroller.scroll(lines * -1);
    render(Scroll);
}

void Render::scrollDown(int lines) {
    scroller.scroll(lines);
    render(Scroll);
}
